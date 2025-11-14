#include "Payment.h"

Payment::Payment(double amount)
    : amount(amount) {}

Payment::~Payment() = default;

double Payment::getAmount() const
{
    return amount;
}

void Payment::setAmount(double a)
{
    amount = a;
}

CashPayment::CashPayment(double amount, double cashGiven)
    : Payment(amount), cashGiven(cashGiven) {}

double CashPayment::getCashGiven() const
{
    return cashGiven;
}

double CashPayment::getChange() const
{
    return cashGiven - amount;
}

QString CashPayment::getMethodName() const
{
    return "Tiền Mặt";
}


CardPayment::CardPayment(double amount, const QString& cardNumber, const QString& bankName)
    : Payment(amount), cardNumber(cardNumber), bankName(bankName) {}

const QString& CardPayment::getCardNumber() const
{
    return cardNumber;
}

const QString& CardPayment::getBankName() const
{
    return bankName;
}

QString CardPayment::getMethodName() const
{
    return "Thẻ";
}

