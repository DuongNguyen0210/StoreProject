#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString> // Đã đổi

class Payment {
protected:
    double amount;

public:
    Payment(double amount = 0.0);
    virtual ~Payment();

    double getAmount() const;
    void setAmount(double a);

    virtual QString getMethodName() const = 0; // Đã đổi
    virtual void printInfo() const;
};

class CashPayment : public Payment {
private:
    double cashGiven;

public:
    CashPayment(double amount = 0.0,
                double cashGiven = 0.0);

    double getCashGiven() const;
    double getChange() const;

    QString getMethodName() const override; // Đã đổi
    void printInfo() const override;
};

class CardPayment : public Payment {
private:
    QString cardNumber; // Đã đổi
    QString bankName;   // Đã đổi

public:
    CardPayment(double amount = 0.0,
                const QString& cardNumber = "", // Đã đổi
                const QString& bankName = "");  // Đã đổi

    const QString& getCardNumber() const; // Đã đổi
    const QString& getBankName() const;   // Đã đổi

    QString getMethodName() const override; // Đã đổi
    void printInfo() const override;
};

#endif
