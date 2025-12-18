#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <cstddef>
#include <cctype>
#include <QString>
#include <QDebug>
#include <QtGlobal>

template<typename V>
class HashTable
{
private:
    struct Node
    {
        QString key;
        V value;
        Node* next;

        Node(const QString& k, const V& v, Node* n = nullptr)
            : key(k), value(v), next(n) {}
    };

    Node** buckets;
    int bucketCount;
    int count;

    static constexpr size_t MOD = 1000000007ULL;

    static QString normalizeKey(const QString& key)
    {
        return key.toLower().trimmed();
    }

    size_t hashKey(const QString& rawKey) const
    {
        QString s = normalizeKey(rawKey);
        size_t h = 0;
        for (QChar c : std::as_const(s))
            h = (h * 311ULL + c.unicode()) % MOD;
        return h;
    }

    int indexForKey(const QString& rawKey) const
    {
        if (bucketCount <= 0)
            return 0;
        return static_cast<int>(hashKey(rawKey) % static_cast<size_t>(bucketCount));
    }

public:
    explicit HashTable(int bucketCount = 101) : buckets(nullptr), bucketCount(bucketCount), count(0)
    {
        buckets = new Node*[bucketCount];
        for (int i = 0; i < bucketCount; ++i)
            buckets[i] = nullptr;
    }

    ~HashTable()
    {
        clear();
        delete[] buckets;
    }

    void clear()
    {
        for (int i = 0; i < bucketCount; ++i)
        {
            Node* cur = buckets[i];
            while (cur)
            {
                Node* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
            buckets[i] = nullptr;
        }
        count = 0;
    }

    int size() const
    {
        return count;
    }

    bool containsKey(const QString& key) const
    {
        QString norm = normalizeKey(key);
        int idx = indexForKey(key);
        Node* cur = buckets[idx];
        while (cur)
        {
            if (normalizeKey(cur->key) == norm)
                return true;
            cur = cur->next;
        }
        return false;
    }

    bool insert(const QString& key, const V& value)
    {
        int idx = indexForKey(normalizeKey(key));
        buckets[idx] = new Node(key, value, buckets[idx]);
        ++count;
        return true;
    }

    V getFirst(const QString& key) const
    {
        QString norm = normalizeKey(key);
        int idx = indexForKey(key);
        Node* cur = buckets[idx];
        while (cur)
        {
            if (normalizeKey(cur->key) == norm)
                return cur->value;
            cur = cur->next;
        }
        return V();
    }

    bool remove(const QString& key)
    {
        QString norm = normalizeKey(key);
        int idx = indexForKey(key);
        Node* cur = buckets[idx];
        Node* prev = nullptr;

        while (cur)
        {
            if (normalizeKey(cur->key) == norm)
            {
                if (prev) prev->next = cur->next;
                else buckets[idx] = cur->next;
                delete cur;
                --count;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    template<typename EqualFunc, typename MergeFunc>
    void insertOrMerge(const QString& key, const V& value, EqualFunc isSame, MergeFunc merge)
    {
        QString norm = normalizeKey(key);
        int idx = indexForKey(key);

        Node* cur = buckets[idx];
        while (cur)
        {
            if (normalizeKey(cur->key) == norm)
            {
                if (isSame(cur->value, value))
                {
                    merge(cur->value, value);
                    return;
                }
            }
            cur = cur->next;
        }

        buckets[idx] = new Node(key, value, buckets[idx]);
        ++count;
    }

    template<typename Func>
    void forEach(Func func)
    {
        for (int i = 0; i < bucketCount; ++i)
        {
            Node* cur = buckets[i];
            while (cur)
            {
                func(cur->key, cur->value);
                cur = cur->next;
            }
        }
    }

    template<typename Func>
    void forEach(Func func) const
    {
        for (int i = 0; i < bucketCount; ++i)
        {
            Node* cur = buckets[i];
            while (cur)
            {
                func(cur->key, cur->value);
                cur = cur->next;
            }
        }
    }

    template<typename Func>
    void forEachInKeyGroup(const QString& key, Func func) const
    {
        QString norm = normalizeKey(key);
        int idx = indexForKey(norm);
        Node* cur = buckets[idx];
        while (cur)
        {
            func(cur->key, cur->value);
            cur = cur->next;
        }
    }
};

#endif
