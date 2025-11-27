#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QSet>
#include <iostream>

using namespace std;

class Product
{
protected:
    QString id;
    QString name;
    double basePrice;
    int quantity;

    static QSet<int> usedIds;
    static QString generateId();

public:
    Product(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0);

    virtual ~Product();

    const QString& getId() const;
    const QString& getName() const;
    double getBasePrice() const;
    int getQuantity() const;

    void setId(const QString& i);
    void setName(const QString& n);
    void setBasePrice(double p);
    void setQuantity(int q);

    virtual double calcFinalPrice() const = 0;

    static void registerUsedId(const QString& id);
    static void unregisterUsedId(const QString& id);
};

#endif
