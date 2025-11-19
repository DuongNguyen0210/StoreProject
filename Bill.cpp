#include "Bill.h"
#include "Customer.h"
#include "User.h"
#include "Exceptions.h"
#include <sstream>

int Bill::nextId = 0;

QString Bill::generateId()
{
    QString id = QString("B%1").arg(nextId++);
    return id;
}

Bill::Bill(Customer* customer, const QString& id, User* createdBy, const QDateTime& createdDate)
    : customer(customer), payment(nullptr), discountPercent(0.0), check(false), createdBy(createdBy)
{
    if (id.isEmpty())
        this->id = generateId();
    else
        this->id = id;

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

void Bill::addItem(Product* p, int quantity)
{
    if (!p)
        throw InvalidInputException("Product is null");
    if (quantity <= 0)
        throw InvalidInputException("Quantity must be > 0");
    if (p->getQuantity() < quantity)
        throw OutOfStockException("Product " + p->getName() + " only has " + QString::number(p->getQuantity()));
    p->setQuantity(p->getQuantity() - quantity);
    for(size_t i = 0; i < items.size(); i++)
        if(items[i].getProduct()->getId() == p->getId())
        {
            items[i].setQuantity(items[i].getQuantity() + quantity);
            return;
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

bool Bill::applyPointsDiscount(int pointsRequired)
{
    if (discountPercent > 0.0)
        return false;

    if (customer == nullptr)
        return false;

    if (customer->getPoints() < pointsRequired)
        return false;
    customer->setPoints(customer->getPoints() - pointsRequired);
    this->discountPercent = 0.02;

    return true;
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
