#include "models/Customer.h"
#include <QDebug>

QSet<int> Customer::usedIds;

QString Customer::generateId()
{
    int mex = 0;
    while (usedIds.contains(mex))
        mex++;
    usedIds.insert(mex);

    return QString("C%1").arg(mex);
}

void Customer::registerUsedId(const QString& id)
{
    if (id.startsWith('C', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
            usedIds.insert(idNum);
    }
}

Customer::Customer(const QString& id, const QString& name, const QString& phone, int points)
    : name(name), phone(phone), points(points), tier("Bronze")
{
    if (id.isEmpty())
        this->id = generateId();
    else
    {
        this->id = id;
        registerUsedId(id);
    }
    updateTierBasedOnPoints();
}

const QString& Customer::getId() const
{
    return id;
}

const QString& Customer::getName() const
{
    return name;
}

const QString& Customer::getPhone() const
{
    return phone;
}

QString Customer::getTier() const
{
    return tier;
}

void Customer::updateTierBasedOnPoints()
{
    QString newTier = "Bronze";
    if (points >= 30000)
        newTier = "Diamond";
    else if (points >= 10000)
        newTier = "Gold";
    else if (points >= 5000)
        newTier = "Silver";
    tier = newTier;
}

double Customer::getTierDiscountPercent() const
{
    if (points >= 30000) return 10.0;
    if (points >= 10000) return 5.0;
    if (points >= 5000) return 2.0;
    return 0.0;
}

int Customer::getPoints() const
{
    return points;
}

void Customer::setName(const QString& n)
{
    name = n;
}

void Customer::setPhone(const QString& p)
{
    phone = p;
}

void Customer::setPoints(int p)
{
    points = p;
    updateTierBasedOnPoints();
}

void Customer::addPoints(int p)
{
    points += p;
    updateTierBasedOnPoints();
}
