#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>
#include <QSet>
#include <QDateTime>

class Customer
{
private:
    QString id;
    QString name;
    QString phone;
    QString email;
    int points;
    QString m_tier;  // Permanent tier - can only upgrade, never downgrade
    QDateTime lastVisit;

    static QSet<int> usedIds;
    static QString generateId();

public:
    Customer(const QString& id = "", const QString& name = "", const QString& phone = "", const QString& email = "", int points = 0);

    const QString& getId() const;
    const QString& getName() const;
    const QString& getPhone() const;
    const QString& getEmail() const;
    int getPoints() const;
    const QDateTime& getLastVisit() const;
    QString getTier() const;
    double getTierDiscountPercent() const;  // Get tier discount %

    void setName(const QString& n);
    void setPhone(const QString& p);
    void setEmail(const QString& e);
    void setPoints(int p);
    void addPoints(int p);
    void setTier(const QString& tier);  // Load tier from file
    void updateLastVisit();
    void updateTierBasedOnPoints();  // Update tier based on current points (upgrade only)

    static void registerUsedId(const QString& id);
    static void unregisterUsedId(const QString& id);
};

#endif
