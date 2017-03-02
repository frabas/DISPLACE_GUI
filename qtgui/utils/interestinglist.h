#ifndef INTERESTINGLIST_H
#define INTERESTINGLIST_H

#include <QList>

template<typename T>
class InterestingList {
private:
    QList<T> list;
public:
    InterestingList() {}

    void set(T n) {
        if (!list.contains(n))
            list.append(n);
            qSort(list);
    }

    void rem(T n) {
        list.removeAll(n);
    }

    bool has(T n) const {
        return list.contains(n);
    }

    void clear() {
        list.clear();
    }
};

#endif // INTERESTINGLIST_H
