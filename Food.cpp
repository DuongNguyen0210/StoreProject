#include "Food.h"
#include <iostream>
#include <QString>
using namespace std;

Food::Food(const QString& id, const QString& name, double basePrice, int quantity, const QString& expiryDate)
    : Product(id, name, basePrice, quantity), expiryDate(expiryDate) {}

const QString& Food::getExpiryDate() const
{
    return expiryDate;
}

void Food::setExpiryDate(const QString& d)
{
    expiryDate = d;
}

double Food::calcFinalPrice() const
{
    return basePrice;
}

