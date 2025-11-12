#include "User.h"
#include <QDebug>

int User::nextId = 1;

QString User::generateId()
{
    return QString("U%1").arg(nextId++);
}

User::User(const QString& id, const QString& name) : name(name)
{
    if (id.isEmpty())
        this->id = generateId();
    else
        this->id = id;
}

User::~User() = default;

const QString& User::getId() const
{
    return id;
}

const QString& User::getName() const
{
    return name;
}

void User::setName(const QString& n)
{
    name = n;
}

