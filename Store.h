#ifndef STORE_H
#define STORE_H

#include <QString>
#include <vector>
#include <QVector>
#include "HashTable.h"
#include "Bill.h"
#include <functional>

class Product;
class Customer;
class User;

class Store
{
private:
    QString name;
    double totalRevenue;

    HashTable<Product*> productByName;
    HashTable<Product*> productById;

    HashTable<Customer*> customerByName;
    HashTable<Customer*> customerById;
    HashTable<Customer*> customerByPhone;

    HashTable<User*> userByName;
    HashTable<User*> userById;

    std::vector<Bill*> billHistory;

    static QString normalizeName(const QString& s);

public:
    Store(const QString& name = "Store");
    ~Store();

    const QString& getName() const;
    void setName(const QString& n);

    void addProduct(Product* p);
    Product* findProductByName(const QString& name) const;
    Product* findProductById(const QString& id) const;

    template<typename Func>
    void forEachProduct(Func f) const
    {
        productById.forEach(f);
    }

    template<typename Func>
    void forEachProductByName(const QString& name, Func f) const
    {
        productByName.forEachInKeyGroup(name, f);
    }

    void addCustomer(Customer* c);
    Customer* findCustomerByName(const QString& name) const;
    Customer* findCustomerById(const QString& id) const;
    Customer* findCustomerByPhone(const QString& phone) const;

    void addRevenue(double amount);
    double getTotalRevenue() const;

    template<typename Func>
    void forEachCustomer(Func f) const
    {
        customerById.forEach(f);
    }

    void addUser(User* u);
    User* findUserByName(const QString& name) const;
    User* findUserById(const QString& id) const;

    template<typename Func>
    void forEachUser(Func f) const
    {
        userById.forEach(f);
    }

    void addBillToHistory(Bill* bill);
    const std::vector<Bill*>& getBillHistory() const;

    // Auto-generate product IDs
    QString generateFoodId();
    QString generateBeverageId();
    QString generateHouseholdId();
};

#endif
