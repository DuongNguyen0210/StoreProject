#ifndef BILL_H
#define BILL_H

#include <QString>
#include <QDateTime>
#include <vector>
#include <iostream>
#include "BillItem.h"
#include "Payment.h"

class Customer;
class User;

class Bill
{
private:
    QString id;
    Customer* customer;
    std::vector<BillItem> items;
    Payment* payment;
    double discountPercent;
    bool check;
    QDateTime createdDate;
    User* createdBy;

    static int nextId;
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

    double getSubTotal() const;
    double getTotal() const;

    bool applyPointsDiscount(int pointsRequired);

    void setCheck(const bool x);
    bool getCheck();

    void setPayment(Payment* p);
    Payment* getPayment() const;


    const QDateTime& getCreatedDate() const;
    User* getCreatedBy() const;
    void setCreatedBy(User* user);
};

#endif
