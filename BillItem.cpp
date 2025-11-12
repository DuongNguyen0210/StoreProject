#include "BillItem.h"

using namespace std;

BillItem::BillItem(Product* p,
                   int quantity,
                   double unitPrice)
    : product(p),
    quantity(quantity),
    unitPrice(unitPrice) {
}

Product* BillItem::getProduct() const {
    return product;
}

int BillItem::getQuantity() const {
    return quantity;
}

double BillItem::getUnitPrice() const {
    return unitPrice;
}

double BillItem::getLineTotal() const {
    return unitPrice * quantity;
}

void BillItem::setQuantity(int x)
{
    quantity = x;
}
