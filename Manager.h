#ifndef MANAGER_H
#define MANAGER_H

#include "User.h"
#include <QString> // Thêm vào

class Manager : public User {
public:
    Manager(const QString& id = "", // Đã đổi
            const QString& name = ""); // Đã đổi

    QString getRole() const override; // Đã đổi
};

#endif
