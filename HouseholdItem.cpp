#include "HouseholdItem.h"
#include <iostream>

using namespace std;

HouseholdItem::HouseholdItem(const QString &id, const QString &name, double basePrice, int quantity, const QString& imageFile, int warrantyMonths)
    : Product(id, name, basePrice, quantity, imageFile), warrantyMonths(warrantyMonths) {}

int HouseholdItem::getWarrantyMonths() const
{
    return warrantyMonths;
}

void HouseholdItem::setWarrantyMonths(int m)
{
    warrantyMonths = m;
}

double HouseholdItem::calcFinalPrice() const
{
    return basePrice;
}

