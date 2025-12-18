#ifndef USER_H
#define USER_H

#include <QString>
#include <QSet>

class User {
protected:
    QString id;
    QString name;
    QString password;

    static QSet<int> usedIds;  // Lưu các ID đã dùng
    static QString generateId();  // Tạo ID theo MEX

public:
    User(const QString& id = "", const QString& name = "", const QString& password = "");

    virtual ~User();

    const QString& getId() const;
    const QString& getName() const;
    const QString& getPassword() const;

    void setName(const QString& n);
    void setPassword(const QString& p);

    virtual QString getRole() const = 0;

    // Đăng ký ID đã sử dụng
    static void registerUsedId(const QString& id);
    // Hủy đăng ký ID
    static void unregisterUsedId(const QString& id);
};

#endif
