#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>

class Payment
{
public:
    Payment() = default;
    virtual ~Payment() = default;

    virtual QString getMethodName() const = 0;
};

class CashPayment : public Payment
{
public:
    CashPayment() = default;

    QString getMethodName() const override;
};

class CardPayment : public Payment
{
public:
    CardPayment() = default;

    QString getMethodName() const override;
};

#endif
