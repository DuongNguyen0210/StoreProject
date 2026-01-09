#ifndef STOREPERSISTENCE_H
#define STOREPERSISTENCE_H

#include <QString>

class Store;

class StorePersistence
{
public:

    static bool save(const Store &store, const QString &filePath);
    static bool load(Store &store, const QString &filePath);
};

#endif
