#include "Product.h"
#include <QString>
using namespace std;

int Product::nextId = 1;

QString Product::generateId()
{
    QString id = QString("P%1").arg(nextId++);
    return id;
}

Product::Product(const QString& id, const QString& name, double basePrice, int quantity, const QString& imageFile)
    : basePrice(basePrice), quantity(quantity), imageFile(imageFile) // <-- THÊM ", imageFile(imageFile)"
{
    if (id.isEmpty())
        this->id = generateId();
    else
        this->id = id;
    this->name = name;
}

Product::~Product() = default;

const QString& Product::getId() const
{
    return id;
}

const QString& Product::getName() const
{
    return name;
}

double Product::getBasePrice() const
{
    return basePrice;
}

int Product::getQuantity() const
{
    return quantity;
}

void Product::setId(const QString& i)
{
    id = i;
}

void Product::setName(const QString& n)
{
    name = n;
}

void Product::setBasePrice(double p)
{
    basePrice = p;
}

void Product::setQuantity(int q)
{
    quantity = q;
}

const QString& Product::getImageFile() const
{
    return imageFile;
}

