#ifndef FOOD_H
#define FOOD_H

#include "Product.h"
#include <QString>

// ✅ COMPILER GUARD: Removed 'using namespace std;' from header

class Food : public Product
{
private:
    QString expiryDate;

public:
    Food(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, const QString& expiryDate = "");

    const QString& getExpiryDate() const;
    void setExpiryDate(const QString& d);

    double calcFinalPrice() const override;
};

#endif
