#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>
#include <QSet>

class Customer
{
private:
    QString id;
    QString name;
    QString phone;
    int points;

    static QSet<int> usedIds;
    static QString generateId();

public:
    Customer(const QString& id = "", const QString& name = "", const QString& phone = "", int points = 0);

    const QString& getId() const;
    const QString& getName() const;
    const QString& getPhone() const;
    int getPoints() const;

    void setName(const QString& n);
    void setPhone(const QString& p);
    void setPoints(int p);
    void addPoints(int p);

    static void registerUsedId(const QString& id);
    static void unregisterUsedId(const QString& id);
};

#endif
