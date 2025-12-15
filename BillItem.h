#ifndef BILLITEM_H
#define BILLITEM_H

#include "Product.h"

// ✅ COMPILER GUARD: Removed 'using namespace std;' from header

class BillItem
{
private:
    Product* product;
    int quantity;
    double unitPrice;

public:
    BillItem(Product* p = nullptr, int quantity = 0, double unitPrice = 0.0);

    Product* getProduct() const;
    int getQuantity() const;
    double getUnitPrice() const;

    double getLineTotal() const;

    void setQuantity(int x);
};

#endif
