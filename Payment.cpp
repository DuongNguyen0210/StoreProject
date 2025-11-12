#include "Payment.h"
#include <QDebug> // Dùng qDebug cho Qt

Payment::Payment(double amount)
    : amount(amount) {
}

Payment::~Payment() = default;

double Payment::getAmount() const {
    return amount;
}

void Payment::setAmount(double a) {
    amount = a;
}

void Payment::printInfo() const {
    qDebug() << "Payment[Amount=" << amount << "]";
}

// CashPayment

CashPayment::CashPayment(double amount,
                         double cashGiven)
    : Payment(amount),
    cashGiven(cashGiven) {
}

double CashPayment::getCashGiven() const {
    return cashGiven;
}

double CashPayment::getChange() const {
    return cashGiven - amount;
}

QString CashPayment::getMethodName() const { // Đã đổi
    return "Cash";
}

void CashPayment::printInfo() const {
    qDebug() << "CashPayment[Amount=" << amount
             << ", CashGiven=" << cashGiven
             << ", Change=" << getChange()
             << "]";
}

// CardPayment

CardPayment::CardPayment(double amount,
                         const QString& cardNumber, // Đã đổi
                         const QString& bankName)   // Đã đổi
    : Payment(amount),
    cardNumber(cardNumber),
    bankName(bankName) {
}

const QString& CardPayment::getCardNumber() const { // Đã đổi
    return cardNumber;
}

const QString& CardPayment::getBankName() const { // Đã đổi
    return bankName;
}

QString CardPayment::getMethodName() const { // Đã đổi
    return "Card";
}

void CardPayment::printInfo() const {
    qDebug() << "CardPayment[Amount=" << amount
             << ", CardNumber=" << cardNumber
             << ", Bank=" << bankName
             << "]";
}
