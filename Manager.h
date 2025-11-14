#ifndef MANAGER_H
#define MANAGER_H

#include "User.h"
#include <QString>

class Manager : public User
{
public:
    Manager(const QString& id = "", const QString& name = "");
    QString getRole() const override;
};

#endif
