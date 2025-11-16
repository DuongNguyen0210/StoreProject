#include "Beverage.h"
#include <iostream>
#include <QString>

using namespace std;

Beverage::Beverage(const QString& id, const QString& name, double basePrice, int quantity, const QString& imageFile, double volume) : Product(id, name, basePrice, quantity, imageFile), volume(volume) {}

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

