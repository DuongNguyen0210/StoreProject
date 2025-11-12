#ifndef CASHPAYMENT_H
#define CASHPAYMENT_H

#include "Payment.h"

class CashPayment : public Payment
{
private:
    double cashGiven;

public:
    CashPayment(double amount = 0.0, double cashGiven = 0.0);
    bool process() override;
    void printInfo() const override;
};

#endif
