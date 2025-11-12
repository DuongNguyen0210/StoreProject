#include "Cashier.h"

Cashier::Cashier(const QString& id, // Đã đổi
                 const QString& name) // Đã đổi
    : User(id, name) {
}

QString Cashier::getRole() const { // Đã đổi
    return "Cashier";
}
