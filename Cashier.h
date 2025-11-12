#ifndef CASHIER_H
#define CASHIER_H

#include "User.h"
#include <QString> // Thêm vào

class Cashier : public User {
public:
    Cashier(const QString& id = "", // Đã đổi
            const QString& name = ""); // Đã đổi

    QString getRole() const override; // Đã đổi
};

#endif
