#include "models/BillItem.h"

using namespace std;

BillItem::BillItem(Product* p, int quantity, double unitPrice, double importPrice)
    : product(p), quantity(quantity), unitPrice(unitPrice), importPrice(importPrice) {}

Product* BillItem::getProduct() const
{
    return product;
}

int BillItem::getQuantity() const
{
    return quantity;
}

double BillItem::getUnitPrice() const
{
    return unitPrice;
}

double BillItem::getImportPrice() const
{
    return importPrice;
}

void BillItem::setImportPrice(double price)
{
    importPrice = price;
}

double BillItem::getLineTotal() const
{
    return unitPrice * quantity;
}

void BillItem::setQuantity(int x)
{
    quantity = x;
}

void BillItem::setUnitPrice(double price)
{
    unitPrice = price;
}
