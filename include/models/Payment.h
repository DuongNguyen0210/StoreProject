#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>

class Payment
{
protected:
    double amount;

public:
    Payment(double amount = 0.0);
    virtual ~Payment();

    double getAmount() const;
    void setAmount(double a);

    virtual QString getMethodName() const = 0;
};

class CashPayment : public Payment
{
private:
    double cashGiven;

public:
    CashPayment(double amount = 0.0, double cashGiven = 0.0);

    double getCashGiven() const;
    double getChange() const;

    QString getMethodName() const override;
};

class CardPayment : public Payment
{
private:
    QString cardNumber;
    QString bankName;

public:
    CardPayment(double amount = 0.0, const QString& cardNumber = "", const QString& bankName = "");

    const QString& getCardNumber() const;
    const QString& getBankName() const;

    QString getMethodName() const override;
};

#endif
