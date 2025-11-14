#ifndef CASHIER_H
#define CASHIER_H

#include "User.h"
#include <QString>

class Cashier : public User
{
public:
    Cashier(const QString& id = "", const QString& name = "");
    QString getRole() const override;
};

#endif
