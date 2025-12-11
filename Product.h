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
    double basePrice;        // Giá bán (Selling Price)
    double importPrice;      // Giá gốc (Import Price)
    double profitMargin;     // % Lợi nhuận (Profit Margin %)
    int quantity;

    static QSet<int> usedIds;
    static QString generateId();

public:
    Product(const QString& id = "", const QString& name = "", double basePrice = 0.0, int quantity = 0, 
            double importPrice = 0.0, double profitMargin = 0.0);

    virtual ~Product();

    const QString& getId() const;
    const QString& getName() const;
    double getBasePrice() const;        // Giá bán
    double getImportPrice() const;      // Giá gốc
    double getProfitMargin() const;     // % Lợi nhuận
    int getQuantity() const;

    void setId(const QString& i);
    void setName(const QString& n);
    void setBasePrice(double p);
    void setImportPrice(double ip);
    void setProfitMargin(double pm);
    void setQuantity(int q);

    virtual double calcFinalPrice() const = 0;

    static void registerUsedId(const QString& id);
    static void unregisterUsedId(const QString& id);
};

#endif
