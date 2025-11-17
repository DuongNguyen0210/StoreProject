#include "Manager.h"

Manager::Manager(const QString& id, const QString& name, const QString& password)
    : User(id, name, password) {}

QString Manager::getRole() const
{
    return "Manager";
}
