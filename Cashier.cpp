#include "Cashier.h"

Cashier::Cashier(const QString& id, const QString& name)
    : User(id, name) {}

QString Cashier::getRole() const
{
    return "Cashier";
}
