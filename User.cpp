#include "User.h"
#include <QDebug>

int User::nextId = 0;

QString User::generateId()
{
    return QString("U%1").arg(nextId++);
}

User::User(const QString& id, const QString& name, const QString& password)
    : name(name), password(password)
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

const QString& User::getPassword() const
{
    return password;
}

void User::setName(const QString& n)
{
    name = n;
}

void User::setPassword(const QString& p)
{
    password = p;
}
