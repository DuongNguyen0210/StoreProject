#include "Customer.h"
#include <QDebug>

int Customer::nextId = 1;

QString Customer::generateId()
{
    return QString("C%1").arg(nextId++);
}

Customer::Customer(const QString& id, const QString& name, const QString& phone, int points)
    : name(name), phone(phone), points(points)
{
    if (id.isEmpty())
        this->id = generateId();
    else
        this->id = id;
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
}

void Customer::addPoints(int p)
{
    points += p;
}
