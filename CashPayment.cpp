#include "CashPayment.h"
#include <iostream>
using namespace std;

CashPayment::CashPayment(double amount, double cashGiven)
    : Payment(amount), cashGiven(cashGiven) {}

bool CashPayment::process()
{
    if (cashGiven < amount)
    {
        throw InvalidDataException("Tien khach dua khong du.");
    }
    double change = cashGiven - amount;
    cout << "Thanh toan tien mat thanh cong. Tien thoi: " << change << "\n";
    return true;
}

void CashPayment::printInfo() const
{
    cout << "CashPayment[Amount =" << amount << ", CashGiven =" << cashGiven << "]\n";
}
