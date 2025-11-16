#ifndef BEVERAGE_H
#define BEVERAGE_H

#include "Product.h"
#include <QString>
using namespace std;

class Beverage : public Product
{
private:
    double volume;

public:
    Beverage(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, const QString& imageFile = "", double volume = 0.0);

    double getVolume() const;
    void setVolume(double v);

    double calcFinalPrice() const override;
};

#endif
