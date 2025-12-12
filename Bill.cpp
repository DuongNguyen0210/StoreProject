#include "Bill.h"
#include "Customer.h"
#include "User.h"
#include "Exceptions.h"
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
    : customer(customer), payment(nullptr), discountPercent(0.0), check(false), createdBy(createdBy)
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
    return subTotal * (1.0 - discountPercent);
}

// Trong file Bill.cpp

bool Bill::applyPointsDiscount(int pointsRequired)
{
    if (discountPercent > 0.0 || customer == nullptr)
        return false;

    int currentPoints = customer->getPoints();
    double subTotal = getSubTotal();

    // 1. Kiểm tra điểm tối thiểu (10 điểm = 1000đ)
    if (currentPoints < 10)
        return false;

    // Quy tắc: Hóa đơn phải còn ít nhất 1.000đ sau khi giảm
    // Nghĩa là số tiền tối đa được phép giảm = Tổng tiền - 1.000đ
    double maxAllowedDiscount = subTotal - 1000.0;

    // Nếu hóa đơn quá nhỏ (<= 1000đ), không cho giảm
    if (maxAllowedDiscount <= 0)
        return false;
    if (maxAllowedDiscount <= 0) return false;

    // Tính giá trị tiền của toàn bộ số điểm khách đang có
    double pointsValueInMoney = currentPoints * 100.0;

    // Số tiền giảm thực tế là số nhỏ hơn giữa (Tiền của điểm) và (Tiền được phép giảm)
    double actualDiscountMoney = (pointsValueInMoney > maxAllowedDiscount)
                                     ? maxAllowedDiscount
                                     : pointsValueInMoney;

    // Tính ra số điểm cần dùng (chia 100)
   int actualPointsToUse = qRound(actualDiscountMoney / 100.0);

    // Tính lại chính xác số tiền giảm từ số điểm chẵn
    double finalDiscountMoney = actualPointsToUse * 100.0;

    if (actualPointsToUse > 0 && finalDiscountMoney > 0)
    {
        customer->setPoints(customer->getPoints() - actualPointsToUse);

        this->pointsUsed = actualPointsToUse;
        this->discountPercent = finalDiscountMoney / subTotal;
        this->check = true;
        return true;
    }

    return false;
}


void Bill::removePointsDiscount()
{
    if (!check || customer == nullptr) return;

    if (pointsUsed > 0)
    {
        customer->setPoints(customer->getPoints() + pointsUsed);
    }

    this->pointsUsed = 0;
    this->discountPercent = 0.0;
    this->check = false;
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

void Bill::setCheck(const bool x)
{
    check = x;
}

bool Bill::getCheck()
{
    return check;
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
