#ifndef USER_H
#define USER_H

#include <QString>

class User {
protected:
    QString id;
    QString name;

    static int nextId;
    static QString generateId();

public:
    User(const QString& id = "", const QString& name = "");

    virtual ~User();

    const QString& getId() const;
    const QString& getName() const;

    void setName(const QString& n);

    virtual QString getRole() const = 0;
};

#endif
