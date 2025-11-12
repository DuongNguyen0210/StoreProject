#include "Manager.h"

Manager::Manager(const QString& id, // Đã đổi
                 const QString& name) // Đã đổi
    : User(id, name) {
}

QString Manager::getRole() const { // Đã đổi
    return "Manager";
}
