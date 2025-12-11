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

Customer::Customer(const QString& id, const QString& name, const QString& phone, int points)
    : name(name), phone(phone), points(points)
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

// 🛡️ SECURITY FIX: Prevent negative points
void Customer::setPoints(int p)
{
    if (p < 0) {
        qDebug() << "❌ CHẶN: Không cho phép điểm âm! Giữ nguyên điểm cũ:" << points;
        return;
    }
    points = p;
}

// 🛡️ SECURITY FIX: Prevent points from going negative
void Customer::addPoints(int p)
{
    int newPoints = points + p;
    if (newPoints < 0) {
        qDebug() << "❌ CHẶN: Cộng/trừ điểm làm tổng âm! Giữ nguyên:" << points;
        return;
    }
    points = newPoints;
}
