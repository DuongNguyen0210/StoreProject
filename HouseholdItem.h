#ifndef HOUSEHOLDITEM_H
#define HOUSEHOLDITEM_H

#include "Product.h"

// ✅ COMPILER GUARD: Removed 'using namespace std;' from header

class HouseholdItem : public Product
{
private:
    int warrantyMonths;

public:
    HouseholdItem(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, int warrantyMonths = 0);

    int getWarrantyMonths() const;
    void setWarrantyMonths(int m);

    double calcFinalPrice() const override;
};

#endif
