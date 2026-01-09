#ifndef MANAGER_H
#define MANAGER_H

#include "models/User.h"
#include <QString>

class Manager : public User
{
public:
    Manager(const QString& id = "", const QString& name = "", const QString& password = "");
    QString getRole() const override;
};

#endif
