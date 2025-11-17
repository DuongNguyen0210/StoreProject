#ifndef USER_H
#define USER_H

#include <QString>

class User {
protected:
    QString id;
    QString name;
    QString password;

    static int nextId;
    static QString generateId();

public:
    User(const QString& id = "", const QString& name = "", const QString& password = "");

    virtual ~User();

    const QString& getId() const;
    const QString& getName() const;
    const QString& getPassword() const;

    void setName(const QString& n);
    void setPassword(const QString& p);

    virtual QString getRole() const = 0;
};

#endif
