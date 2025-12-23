#ifndef BILLITEM_H
#define BILLITEM_H

#include "models/Product.h"

class BillItem
{
private:
    Product* product;
    int quantity;
    double unitPrice;

public:
    BillItem(Product* p = nullptr, int quantity = 0, double unitPrice = 0.0, double importPrice = 0.0);

    Product* getProduct() const;
    int getQuantity() const;
    double getUnitPrice() const;

    double getLineTotal() const;

    void setQuantity(int x);
    void setUnitPrice(double price);

    double getImportPrice() const;
    void setImportPrice(double price);

private:
    double importPrice;
};

#endif
