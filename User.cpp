#include "User.h"
#include <QDebug>

QSet<int> User::usedIds;

QString User::generateId()
{
    int mex = 0;
    while (usedIds.contains(mex))
    {
        mex++;
    }

    usedIds.insert(mex);

    return QString("U%1").arg(mex);
}

void User::registerUsedId(const QString& id)
{
    // Kiểm tra nếu ID có format Uxxxx
    if (id.startsWith('U', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.insert(idNum);
        }
    }
}

void User::unregisterUsedId(const QString& id)
{
    if (id.startsWith('U', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.remove(idNum);
        }
    }
}

User::User(const QString& id, const QString& name, const QString& password)
    : name(name), password(password)
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
}

User::~User()
{
    unregisterUsedId(id);
}

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
