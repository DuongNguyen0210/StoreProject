#include "Store.h"
#include "Product.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"
#include "Customer.h"
#include "User.h"
#include "Exceptions.h"

#include <cctype>
#include <typeinfo>
#include <qstring.h>

static QString toLowerCopy(const QString& s)
{
    return s.toLower();
}

QString Store::normalizeName(const QString& s)
{
    return toLowerCopy(s);
}

Store::Store(const QString& name)
    : name(name),
    totalRevenue(0.0),
    productByName(101), productById(101),
    customerByName(101), customerById(101), customerByPhone(101),
    userByName(101), userById(101)
{}

Store::~Store()
{
    productById.forEach([](const QString&, Product* p) {
        delete p;
    });
    customerById.forEach([](const QString&, Customer* c) {
        delete c;
    });
    userById.forEach([](const QString&, User* u) {
        delete u;
    });
    for (Bill* bill : billHistory)
        delete bill;
    billHistory.clear();
}

const QString& Store::getName() const {
    return name;
}

void Store::setName(const QString& n)
{
    name = n;
}

void Store::addProduct(Product* p)
{
    if (!p) return;

    Product* existingProductToMerge = nullptr;
    productByName.forEachInKeyGroup(p->getName(), [&](const QString&, Product* exist)
                                    {
                                        if (existingProductToMerge)
                                            return;

                                        if (typeid(*exist) != typeid(*p))
                                            return;
                                        if (auto f1 = dynamic_cast<Food*>(exist))
                                        {
                                            auto f2 = dynamic_cast<Food*>(p);
                                            if (f2 && f1->getBasePrice() == f2->getBasePrice() && f1->getExpiryDate() == f2->getExpiryDate())
                                                existingProductToMerge = exist;
                                        }
                                        else if (auto b1 = dynamic_cast<Beverage*>(exist))
                                        {
                                            auto b2 = dynamic_cast<Beverage*>(p);
                                            if (b2 && b1->getBasePrice() == b2->getBasePrice() && b1->getVolume() == b2->getVolume() && b1->getExpiryDate() == b2->getExpiryDate())
                                                existingProductToMerge = exist;
                                        }
                                        else if (auto h1 = dynamic_cast<HouseholdItem*>(exist))
                                        {
                                            auto h2 = dynamic_cast<HouseholdItem*>(p);
                                            if (h2 && h1->getBasePrice() == h2->getBasePrice() && h1->getWarrantyMonths() == h2->getWarrantyMonths())
                                                existingProductToMerge = exist;
                                        }
                                    });

    if (existingProductToMerge)
    {
        existingProductToMerge->setQuantity(existingProductToMerge->getQuantity() + p->getQuantity());
        delete p;
    }
    else
    {
        productById.insert(p->getId(), p);
        productByName.insert(p->getName(), p);
    }
}

Product* Store::findProductByName(const QString& name) const
{
    return productByName.getFirst(name);
}

Product* Store::findProductById(const QString& id) const
{
    return productById.getFirst(id);
}



void Store::addCustomer(Customer* c)
{
    if (!c)
        return;
    if (customerById.containsKey(c->getId()))
    {
        throw DuplicateException("Customer ID already exists: " + c->getId());
        delete c;
    }

    if (!c->getPhone().isEmpty() && customerByPhone.containsKey(c->getPhone()))
    {
        throw DuplicateException("Customer Phone already exists: " + c->getPhone());
        delete c;
    }

    Customer* existingCustomerToMerge = customerByPhone.getFirst(c->getPhone());

    if (existingCustomerToMerge)
    {
        existingCustomerToMerge->setName(c->getName());
        existingCustomerToMerge->addPoints(c->getPoints());
        delete c;
    }
    else
    {
        customerById.insert(c->getId(), c);
        customerByName.insert(c->getName(), c);
        if (!c->getPhone().isEmpty()) {
            customerByPhone.insert(c->getPhone(), c);
        }
    }
}

Customer* Store::findCustomerByName(const QString& name) const
{
    return customerByName.getFirst(name);
}

Customer* Store::findCustomerById(const QString& id) const
{
    return customerById.getFirst(id);
}

Customer* Store::findCustomerByPhone(const QString& phone) const
{
    return customerByPhone.getFirst(phone);
}

void Store::addUser(User* u)
{
    if (!u)
        return;

    if (userById.containsKey(u->getId()))
    {
        throw DuplicateException("User ID already exists: " + u->getId());
        delete u;
    }
    userById.insert(u->getId(), u);
    userByName.insert(u->getName(), u);
}

User* Store::findUserByName(const QString& name) const
{
    return userByName.getFirst(name);
}

User* Store::findUserById(const QString& id) const
{
    return userById.getFirst(id);
}

void Store::addRevenue(double amount)
{
    if (amount > 0)
    {
        totalRevenue += amount;
    }
}

double Store::getTotalRevenue() const
{
    return totalRevenue;
}

void Store::addBillToHistory(Bill* bill)
{
    if (bill)
    {
        billHistory.push_back(bill);
    }
}

const std::vector<Bill*>& Store::getBillHistory() const
{
    return billHistory;
}

QString Store::generateFoodId()
{
    int maxNum = 0;
    productById.forEach([&](const QString& key, Product* p) {
        if (!p || !key.startsWith("F")) return;
        bool ok;
        int num = key.mid(1).toInt(&ok);
        if (ok && num > maxNum) maxNum = num;
    });
    return QString("F%1").arg(maxNum + 1, 3, 10, QChar('0'));
}

QString Store::generateBeverageId()
{
    int maxNum = 0;
    productById.forEach([&](const QString& key, Product* p) {
        if (!p || !key.startsWith("B")) return;
        bool ok;
        int num = key.mid(1).toInt(&ok);
        if (ok && num > maxNum) maxNum = num;
    });
    return QString("B%1").arg(maxNum + 1, 3, 10, QChar('0'));
}

QString Store::generateHouseholdId()
{
    int maxNum = 0;
    productById.forEach([&](const QString& key, Product* p) {
        if (!p || !key.startsWith("H")) return;
        bool ok;
        int num = key.mid(1).toInt(&ok);
        if (ok && num > maxNum) maxNum = num;
    });
    return QString("H%1").arg(maxNum + 1, 3, 10, QChar('0'));
}

