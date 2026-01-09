#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QSet>
#include <iostream>


class Product
{
protected:
    QString id;
    QString name;
    double basePrice;
    double importPrice;
    double profitMargin;
    int quantity;
    bool isActive;

    static QSet<int> usedIds;
    static QString generateId();

public:
    Product(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, 
            double importPrice = 0.0, double profitMargin = 0.0);

    virtual ~Product();
    const QString& getId() const;
    const QString& getName() const;
    double getBasePrice() const;
    double getImportPrice() const;
    double getProfitMargin() const;
    int getQuantity() const;
    bool getIsActive() const;

    void setId(const QString& i);
    void setName(const QString& n);
    void setBasePrice(double p);
    void setImportPrice(double ip);
    void setProfitMargin(double pm);
    void setQuantity(int q);
    void setActive(bool active);

    virtual double calcFinalPrice() const = 0;

    static void registerUsedId(const QString& id);
};

#endif
