#ifndef BEVERAGE_H
#define BEVERAGE_H

#include "Product.h"
#include <QString>

// ✅ COMPILER GUARD: Removed 'using namespace std;' from header

class Beverage : public Product
{
private:
    QString expiryDate;
    double volume;

public:
    Beverage(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, QString expiryDate = "", double volume = 0.0);

    const QString& getExpiryDate() const;
    void setExpiryDate(const QString& d);

    double getVolume() const;
    void setVolume(double v);

    double calcFinalPrice() const override;
};

#endif
