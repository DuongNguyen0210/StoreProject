#include "StorePersistence.h"

#include "Store.h"
#include "Product.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"
#include "Customer.h"
#include "User.h"
#include "Manager.h"
#include "Cashier.h"
#include "Bill.h"
#include "BillItem.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QHash>
#include <QStringList>
#include <QMessageBox>

#include <algorithm>

bool StorePersistence::save(const Store &store, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "[PRODUCTS]\n";
    store.forEachProduct([&](const QString&, Product* p)
                         {
                             if (!p) return;

                             if (auto f = dynamic_cast<Food*>(p))
                             {
                                 out << "Food|"
                                     << f->getId() << '|'
                                     << f->getName() << '|'
                                     << QString::number(f->getBasePrice(), 'f', 2) << '|'
                                     << QString::number(f->getImportPrice(), 'f', 2) << '|'     // Giá gốc
                                     << QString::number(f->getProfitMargin(), 'f', 2) << '|'    // % Lợi nhuận
                                     << f->getQuantity() << '|'
                                     << f->getExpiryDate() << '\n';
                             }
                             else if (auto b = dynamic_cast<Beverage*>(p))
                             {
                                 out << "Beverage|"
                                     << b->getId() << '|'
                                     << b->getName() << '|'
                                     << QString::number(b->getBasePrice(), 'f', 2) << '|'
                                     << QString::number(b->getImportPrice(), 'f', 2) << '|'     // Giá gốc
                                     << QString::number(b->getProfitMargin(), 'f', 2) << '|'    // % Lợi nhuận
                                     << b->getQuantity() << '|'
                                     << b->getExpiryDate() << '|'
                                     << QString::number(b->getVolume(), 'f', 2) << '\n';
                             }
                             else if (auto h = dynamic_cast<HouseholdItem*>(p))
                             {
                                 out << "Household|"
                                     << h->getId() << '|'
                                     << h->getName() << '|'
                                     << QString::number(h->getBasePrice(), 'f', 2) << '|'
                                     << QString::number(h->getImportPrice(), 'f', 2) << '|'     // Giá gốc
                                     << QString::number(h->getProfitMargin(), 'f', 2) << '|'    // % Lợi nhuận
                                     << h->getQuantity() << '|'
                                     << h->getWarrantyMonths() << '\n';
                             }
                         });
    out << "\n";

    out << "[CUSTOMERS]\n";
    store.forEachCustomer([&](const QString&, Customer* c)
                          {
                              if (!c) return;

                              out << c->getId() << '|'
                                  << c->getName() << '|'
                                  << c->getPhone() << '|'
                                  << c->getPoints() << '\n';
                          });
    out << "\n";

    out << "[USERS]\n";
    store.forEachUser([&](const QString&, User* u)
                      {
                          if (!u) return;

                          QString role = u->getRole();
                          out << role << '|'
                              << u->getId() << '|'
                              << u->getName() << '|'
                              << u->getPassword() << '\n';
                      });
    out << "\n";

    out << "[BILLS]\n";
    const std::vector<Bill*>& history = store.getBillHistory();
    for (const Bill* bill : history)
    {
        if (!bill) continue;

        QString billId = bill->getId();
        QString customerId;
        if (bill->getCustomer())
            customerId = bill->getCustomer()->getId();

        QString userId;
        if (bill->getCreatedBy())
            userId = bill->getCreatedBy()->getId();

        QString dateStr = bill->getCreatedDate().toString("yyyy-MM-dd HH:mm:ss");

        out << "BILL|" << billId << '|'
            << dateStr << '|'
            << customerId << '|'
            << userId << '\n';

        const std::vector<BillItem>& items = bill->getItems();
        for (const BillItem& it : items)
        {
            Product* p = it.getProduct();
            QString productId = p ? p->getId() : "";

            out << "ITEM|" << billId << '|'
                << productId << '|'
                << it.getQuantity() << '|'
                << QString::number(it.getUnitPrice(), 'f', 2) << '\n';
        }
    }
    out << "\n";

    out << "[REVENUE]\n";
    out << QString::number(store.getTotalRevenue(), 'f', 2) << "\n";

    return true;
}

bool StorePersistence::load(Store &store, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    enum Section {
        None,
        Products,
        Customers,
        Users,
        Bills,
        Revenue
    };

    Section current = None;

    QHash<QString, Bill*> billMap;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        if (line == "[PRODUCTS]")  { current = Products;  continue; }
        if (line == "[CUSTOMERS]") { current = Customers; continue; }
        if (line == "[USERS]")     { current = Users;     continue; }
        if (line == "[BILLS]")     { current = Bills;     continue; }
        if (line == "[REVENUE]")   { current = Revenue;   continue; }

        if (current == Revenue)
        {
            bool ok = false;
            double totalRev = line.toDouble(&ok);
            if (ok)
                store.addRevenue(totalRev);
            else
            {
                QMessageBox::warning(nullptr, "Lỗi", "Có vấn đề khi đọc doanh thu từ file");
                return false;
            }
            continue;
        }

        QStringList parts = line.split('|');

        switch (current)
        {
        case Products:
        {
            if (parts.size() < 5)
            {
                QMessageBox::warning(nullptr, "Lỗi", "Dữ liệu sản phẩm không đúng định dạng");
                return false;
            }

            QString type = parts[0];
            if (type == "Food")
            {
                // Hỗ trợ cả format cũ (6 fields) và mới (8 fields)
                if (parts.size() < 6) break;
                QString id = parts[1];
                QString name = parts[2];
                double price = parts[3].toDouble();
                
                // Format mới có importPrice và profitMargin
                double importPrice = 0.0;
                double profitMargin = 0.0;
                int quantity = 0;
                QString expiry = "";
                
                if (parts.size() >= 8) {  // Format mới
                    importPrice = parts[4].toDouble();
                    profitMargin = parts[5].toDouble();
                    quantity = parts[6].toInt();
                    expiry = parts[7];
                } else {  // Format cũ - backward compatibility
                    importPrice = price;  // Giả định giá cũ là giá gốc
                    profitMargin = 0.0;
                    quantity = parts[4].toInt();
                    expiry = parts[5];
                }

                Product* p = new Food(id, name, price, quantity, expiry);
                p->setImportPrice(importPrice);
                p->setProfitMargin(profitMargin);
                store.addProduct(p);
            }
            else if (type == "Beverage")
            {
                // Hỗ trợ cả format cũ (7 fields) và mới (9 fields)
                if (parts.size() < 7) break;
                QString id = parts[1];
                QString name = parts[2];
                double price = parts[3].toDouble();
                
                double importPrice = 0.0;
                double profitMargin = 0.0;
                int quantity = 0;
                QString expiry = "";
                double volume = 0.0;
                
                if (parts.size() >= 9) {  // Format mới
                    importPrice = parts[4].toDouble();
                    profitMargin = parts[5].toDouble();
                    quantity = parts[6].toInt();
                    expiry = parts[7];
                    volume = parts[8].toDouble();
                } else {  // Format cũ
                    importPrice = price;
                    profitMargin = 0.0;
                    quantity = parts[4].toInt();
                    expiry = parts[5];
                    volume = parts[6].toDouble();
                }

                Product* p = new Beverage(id, name, price, quantity, expiry, volume);
                p->setImportPrice(importPrice);
                p->setProfitMargin(profitMargin);
                store.addProduct(p);
            }
            else if (type == "Household")
            {
                // Hỗ trợ cả format cũ (6 fields) và mới (8 fields)
                if (parts.size() < 6) break;
                QString id = parts[1];
                QString name = parts[2];
                double price = parts[3].toDouble();
                
                double importPrice = 0.0;
                double profitMargin = 0.0;
                int quantity = 0;
                int warranty = 0;
                
                if (parts.size() >= 8) {  // Format mới
                    importPrice = parts[4].toDouble();
                    profitMargin = parts[5].toDouble();
                    quantity = parts[6].toInt();
                    warranty = parts[7].toInt();
                } else {  // Format cũ
                    importPrice = price;
                    profitMargin = 0.0;
                    quantity = parts[4].toInt();
                    warranty = parts[5].toInt();
                }

                Product* p = new HouseholdItem(id, name, price, quantity, warranty);
                p->setImportPrice(importPrice);
                p->setProfitMargin(profitMargin);
                store.addProduct(p);
            }
            break;
        }

        case Customers:
        {
            if (parts.size() < 4)
            {
                QMessageBox::warning(nullptr, "Lỗi", "Dữ liệu khách hàng không đúng định dạng");
                return false;
            }

            QString id = parts[0];
            QString name = parts[1];
            QString phone = parts[2];
            int points = parts[3].toInt();

            Customer* c = new Customer(id, name, phone, points);
            store.addCustomer(c);
            break;
        }

        case Users:
        {
            if (parts.size() < 4)
            {
                QMessageBox::warning(nullptr, "Lỗi", "Dữ liệu người dùng không đúng định dạng");
                return false;
            }

            QString role = parts[0];
            QString id = parts[1];
            QString name = parts[2].trimmed();  // ✅ SỬA: trim username để tránh khoảng trắng
            QString password = parts[3].trimmed();  // ✅ SỬA: trim password để tránh khoảng trắng

            User* u = nullptr;
            if (role.compare("Manager", Qt::CaseInsensitive) == 0)
                u = new Manager(id, name, password);
            else if (role.compare("Cashier", Qt::CaseInsensitive) == 0)
                u = new Cashier(id, name, password);

            if (u)
            {
                store.addUser(u);
                qDebug() << "✅ Loaded user:" << name << "with password:" << password;
            }
            else
                QMessageBox::warning(nullptr, "Lỗi", "Role người dùng không hợp lệ: " + role);

            break;
        }

        case Bills:
        {
            if (parts.size() < 1)
            {
                QMessageBox::warning(nullptr, "Lỗi", "Dữ liệu hóa đơn không đúng định dạng");
                return false;
            }

            QString tag = parts[0];

            if (tag == "BILL")
            {
                if (parts.size() < 5)
                {
                    QMessageBox::warning(nullptr, "Lỗi", "Dữ liệu BILL không đầy đủ");
                    return false;
                }

                QString id = parts[1];
                QString datetimeStr = parts[2];
                QString customerId = parts[3];
                QString userId = parts[4];

                Customer* c = store.findCustomerById(customerId);
                User* u = store.findUserById(userId);
                QDateTime dt = QDateTime::fromString(datetimeStr, "yyyy-MM-dd HH:mm:ss");

                Bill* b = new Bill(c, id, u, dt);

                billMap[id] = b;

                store.addBillToHistory(b);
            }
            else if (tag == "ITEM")
            {
                if (parts.size() < 5)
                {
                    QMessageBox::warning(nullptr, "Lỗi", "Dữ liệu ITEM không đầy đủ");
                    return false;
                }

                QString billId = parts[1];
                QString productId = parts[2];
                int quantity = parts[3].toInt();
                double unitPrice = parts[4].toDouble();

                Bill* b = billMap.value(billId, nullptr);
                Product* p = store.findProductById(productId);

                if (!b || !p)
                {
                    qDebug() << "Warning: Cannot find Bill" << billId << "or Product" << productId;
                    break;
                }

                const std::vector<BillItem>& constItems = b->getItems();
                auto& items = const_cast<std::vector<BillItem>&>(constItems);
                items.emplace_back(p, quantity, unitPrice);
            }
            break;
        }

        default:
            break;
        }
    }

    return true;
}
