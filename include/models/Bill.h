#ifndef BILL_H
#define BILL_H

#include <QString>
#include <QDateTime>
#include <QSet>
#include <vector>
#include <iostream>
#include "models/BillItem.h"
#include "models/Payment.h"

class Customer;
class User;

class Bill
{
private:
    QString id;
    Customer* customer;
    std::vector<BillItem> items;
    Payment* payment;
    double tierDiscountPercent;
    QDateTime createdDate;
    User* createdBy;

    static QSet<int> usedIds;
    static QString generateId();

public:
    Bill(Customer* customer = nullptr, const QString& id = "", User* createdBy = nullptr, const QDateTime& createdDate = QDateTime());
    ~Bill();

    const QString& getId() const;
    Customer* getCustomer() const;
    void setCustomer(Customer* c);

    void addItem(Product* p, int quantity);
    void removeItem(Product* p);
    const std::vector<BillItem>& getItems() const;
    int getQuantityInCart(const QString& productId) const;

    double getSubTotal() const;
    double getTotal() const;

    void setPayment(Payment* p);
    Payment* getPayment() const;

    const QDateTime& getCreatedDate() const;
    User* getCreatedBy() const;
    void setCreatedBy(User* user);

    static void registerUsedId(const QString& id);
    static void unregisterUsedId(const QString& id);

    double getTierDiscountPercent() const { return tierDiscountPercent; }
    void setTierDiscountPercent(double t) { tierDiscountPercent = t; }

    void clear();
};

#endif
