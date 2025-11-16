#ifndef HOUSEHOLDITEM_H
#define HOUSEHOLDITEM_H

#include "Product.h"

using namespace std;

class HouseholdItem : public Product
{
private:
    int warrantyMonths;

public:
    HouseholdItem(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, const QString& imageFile = "", int warrantyMonths = 0);

    int getWarrantyMonths() const;
    void setWarrantyMonths(int m);

    double calcFinalPrice() const override;
};

#endif
