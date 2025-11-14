#include "Manager.h"

Manager::Manager(const QString& id, const QString& name)
    : User(id, name) {}

QString Manager::getRole() const
{
    return "Manager";
}
