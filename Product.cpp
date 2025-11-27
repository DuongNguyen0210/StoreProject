#include "Product.h"
#include <QString>
using namespace std;

QSet<int> Product::usedIds;

QString Product::generateId()
{
    int mex = 0;
    while (usedIds.contains(mex))
    {
        mex++;
    }
    usedIds.insert(mex);

    return QString("P%1").arg(mex);
}

void Product::registerUsedId(const QString& id)
{
    if (id.startsWith('P', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.insert(idNum);
        }
    }
}

void Product::unregisterUsedId(const QString& id)
{
    if (id.startsWith('P', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.remove(idNum);
        }
    }
}

Product::Product(const QString& id, const QString& name, double basePrice, int quantity)
    : basePrice(basePrice), quantity(quantity)
{
    if (id.isEmpty())
    {
        this->id = generateId();
    }
    else
    {
        this->id = id;
        registerUsedId(id);
    }
    this->name = name;
}

Product::~Product()
{
    unregisterUsedId(id);
}

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
