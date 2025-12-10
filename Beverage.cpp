#include "Beverage.h"
#include <iostream>
#include <QString>

using namespace std;

Beverage::Beverage(const QString& id, const QString& name, double basePrice, int quantity, QString expiryDate, double volume)
    : Product(id, name, basePrice, quantity, 0.0, 0.0), expiryDate(expiryDate), volume(volume) {}

const QString& Beverage::getExpiryDate() const
{
    return expiryDate;
}

void Beverage::setExpiryDate(const QString& d)
{
    expiryDate = d;
}

double Beverage::getVolume() const
{
    return volume;
}

void Beverage::setVolume(double v)
{
    volume = v;
}

double Beverage::calcFinalPrice() const
{
    return basePrice;
}

