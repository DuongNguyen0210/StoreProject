#include "models/Payment.h"

QString CashPayment::getMethodName() const
{
    return "Tiền Mặt";
}

QString CardPayment::getMethodName() const
{
    return "Chuyển Khoản";
}

