#ifndef CARDPAYMENT_H
#define CARDPAYMENT_H

#include "Payment.h"
#include <string>
using namespace std;

class ardPayment : public Payment
{
private:
    string cardNumber;
    string ownerName;

public:
    CardPayment(double amount = 0.0, const string &cardNumber = "", const string &ownerName = "");
    bool process() override;
    void printInfo() const override;
};

#endif
