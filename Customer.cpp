#include "Customer.h"
#include <QDebug>

QSet<int> Customer::usedIds;

QString Customer::generateId()
{
    int mex = 0;
    while (usedIds.contains(mex))
    {
        mex++;
    }
    usedIds.insert(mex);

    return QString("C%1").arg(mex);
}

void Customer::registerUsedId(const QString& id)
{
    if (id.startsWith('C', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.insert(idNum);
        }
    }
}

void Customer::unregisterUsedId(const QString& id)
{
    if (id.startsWith('C', Qt::CaseInsensitive) && id.length() > 1)
    {
        bool ok = false;
        int idNum = id.mid(1).toInt(&ok);

        if (ok && idNum >= 0)
        {
            usedIds.remove(idNum);
        }
    }
}

Customer::Customer(const QString& id, const QString& name, const QString& phone, const QString& email, int points)
    : name(name), phone(phone), email(email), points(points), m_tier("Bronze"), lastVisit(QDateTime::currentDateTime())
{
    if (id.isEmpty())
    {
        this->id = generateId();
    }
    else
    {
        this->id = id;
        // Đăng ký ID này đã được sử dụng
        registerUsedId(id);
    }
    
    // Calculate initial tier based on points
    updateTierBasedOnPoints();
}

const QString& Customer::getId() const
{
    return id;
}

const QString& Customer::getName() const
{
    return name;
}

const QString& Customer::getPhone() const
{
    return phone;
}

const QString& Customer::getEmail() const
{
    return email;
}

const QDateTime& Customer::getLastVisit() const
{
    return lastVisit;
}

QString Customer::getTier() const
{
    return m_tier;  // Return stored tier (never downgrades)
}

void Customer::updateTierBasedOnPoints()
{
    // Calculate tier based on current points
    QString newTier = "Bronze";
    if (points >= 3000) newTier = "Diamond";
    else if (points >= 1000) newTier = "Gold";
    else if (points >= 500) newTier = "Silver";
    
    // Only upgrade, never downgrade
    if (m_tier == "Bronze") m_tier = newTier;
    else if (m_tier == "Silver" && (newTier == "Gold" || newTier == "Diamond")) m_tier = newTier;
    else if (m_tier == "Gold" && newTier == "Diamond") m_tier = newTier;
    // If m_tier == "Diamond", stay Diamond forever
}

double Customer::getTierDiscountPercent() const
{
    if (points >= 3000) return 15.0;
    if (points >= 1000) return 10.0;
    if (points >= 500) return 5.0;
    return 0.0;
}

int Customer::getPoints() const
{
    return points;
}

void Customer::setName(const QString& n)
{
    name = n;
}

void Customer::setPhone(const QString& p)
{
    phone = p;
}

void Customer::setEmail(const QString& e)
{
    email = e;
}

void Customer::updateLastVisit()
{
    lastVisit = QDateTime::currentDateTime();
}

// 🛡️ SECURITY FIX: Prevent negative points
void Customer::setPoints(int p)
{
    if (p < 0) {
        qDebug() << "❌ CHẶN: Không cho phép điểm âm! Giữ nguyên điểm cũ:" << points;
        return;
    }
    points = p;
    updateTierBasedOnPoints();  // Check for tier upgrade
}

// 🛡️ SECURITY FIX: Prevent points from going negative
void Customer::addPoints(int p)
{
    if (points + p < 0) {
        qDebug() << "❌ CHẶN: Không cho phép điểm âm! Giữ nguyên:" << points;
        return;
    }
    points += p;
    updateTierBasedOnPoints();  // Check for tier upgrade
}

void Customer::setTier(const QString& tier)
{
    // Directly set tier (used when loading from file)
    // This bypasses the upgrade-only logic and restores saved tier
    m_tier = tier;
}
