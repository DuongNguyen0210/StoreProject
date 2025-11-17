#include "Cashier.h"

Cashier::Cashier(const QString& id, const QString& name, const QString& password)
    : User(id, name, password) {}

QString Cashier::getRole() const
{
    return "Cashier";
}
