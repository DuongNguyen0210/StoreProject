#include "CardPayment.h"
#include <iostream>
using namespace std;

CardPayment::CardPayment(double amount, const string &cardNumber, const string &ownerName)
    : Payment(amount), cardNumber(cardNumber), ownerName(ownerName)
{
    if (cardNumber.empty())
        throw InvalidDataException("So the khong duoc rong.");
    if (ownerName.empty())
        throw InvalidDataException("Ten chu the khong duoc rong.");
}

bool CardPayment::process()
{
    cout << "Thanh toan the thanh cong.\n";
    return true;
}

void CardPayment::printInfo() const
{
    cout << "CardPayment[Amount =" << amount << ", CardNumber =" << cardNumber << ", Owner =" << ownerName << "]\n";
}
