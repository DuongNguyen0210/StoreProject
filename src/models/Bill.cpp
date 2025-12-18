#include "models/Bill.h"
#include "models/Customer.h"
#include "models/User.h"
#include <sstream>

QSet<int> Bill::usedIds;

QString Bill::generateId()
{
    int mex = 0;
    while (usedIds.contains(mex))
    {
        mex++;
    }
    usedIds.insert(mex);
    return QString("B%1").arg(mex, 3, 10, QChar('0'));
}

void Bill::registerUsedId(const QString& id)
{
    if (id.startsWith('B', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.insert(idNum);
        }
    }
}

void Bill::unregisterUsedId(const QString& id)
{
    if (id.startsWith('B', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.remove(idNum);
        }
    }
}

Bill::Bill(Customer* customer, const QString& id, User* createdBy, const QDateTime& createdDate)
    : customer(customer), payment(nullptr), tierDiscountPercent(0.0), createdBy(createdBy)
{
    if (id.isEmpty())
    {
        this->id = generateId();
    }
    else
    {
        this->id = id;
        registerUsedId(id);
    }

    if (createdDate.isValid())
        this->createdDate = createdDate;
    else
        this->createdDate = QDateTime::currentDateTime();
}

Bill::~Bill()
{
    delete payment;
}

const QString& Bill::getId() const
{
    return id;
}

Customer* Bill::getCustomer() const
{
    return customer;
}

void Bill::setCustomer(Customer* c)
{
    this->customer = c;
}

// ✅ BLUE TEAM FIX: Hardened addItem với validation toàn diện
void Bill::addItem(Product* p, int quantity)
{
    // 🛡️ CHẶN 1: Số lượng phải > 0
    if (quantity <= 0)
    {
        qDebug() << "❌ CHẶN: Số lượng không hợp lệ:" << quantity;
        return; // Không làm gì cả
    }

    // 🛡️ CHẶN 3: Kiểm tra bán khống (Số lượng muốn thêm > Tồn kho hiện tại)
    int availableStock = p->getQuantity();
    if (quantity > availableStock)
    {
        qDebug() << "❌ CHẶN: Bán khống! Kho còn:" << availableStock 
                 << "Muốn thêm:" << quantity;
        return; // Không cho thêm
    }

    // ✅ AN TOÀN: Mới trừ kho (sau khi đã check hết)
    p->setQuantity(p->getQuantity() - quantity);

    // Thêm vào giỏ hoặc tăng số lượng
    for (size_t i = 0; i < items.size(); i++)
    {
        if (items[i].getProduct()->getId() == p->getId())
        {
            items[i].setQuantity(items[i].getQuantity() + quantity);
            return;
        }
    }

    items.emplace_back(p, quantity, p->calcFinalPrice());
}

void Bill::removeItem(Product* p)
{
    for(size_t i = 0; i < items.size(); i++)
    {
        if(items[i].getProduct()->getId() == p->getId())
        {
            p->setQuantity(p->getQuantity() + items[i].getQuantity());
            items.erase(items.begin() + i);
            return;
        }
    }
}

const std::vector<BillItem>& Bill::getItems() const
{
    return items;
}

// ✅ Helper: Lấy số lượng của 1 sản phẩm đang trong giỏ
int Bill::getQuantityInCart(const QString& productId) const
{
    for (const auto& item : items)
    {
        if (item.getProduct()->getId() == productId)
            return item.getQuantity();
    }
    return 0;
}

double Bill::getSubTotal() const
{
    double total = 0.0;
    for (const auto& item : items)
        total += item.getLineTotal();
    return total;
}

double Bill::getTotal() const
{
    double subTotal = getSubTotal();
    
    double tierDiscount = tierDiscountPercent;
    if (tierDiscount == 0.0 && customer != nullptr)
    {
        tierDiscount = customer->getTierDiscountPercent();
    }
    
    // Chỉ áp dụng tier discount
    return subTotal * (1.0 - (tierDiscount / 100.0));
}

void Bill::setPayment(Payment* p)
{
    delete payment;
    payment = p;
    if (payment)
        payment->setAmount(getTotal());
}

Payment* Bill::getPayment() const
{
    return payment;
}



const QDateTime& Bill::getCreatedDate() const
{
    return createdDate;
}

User* Bill::getCreatedBy() const
{
    return createdBy;
}

void Bill::setCreatedBy(User* user)
{
    createdBy = user;
}

void Bill::clear()
{
    items.clear();
    
    customer = nullptr;
    
    delete payment;
    payment = nullptr;
    
    tierDiscountPercent = 0.0;
}
