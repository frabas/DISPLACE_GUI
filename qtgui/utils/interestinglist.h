#ifndef INTERESTINGLIST_H
#define INTERESTINGLIST_H

#include <QList>

template<typename T>
class InterestingList {
private:
    QList<T> mList;
    QString mLabelFormat;
public:
    InterestingList() {}
    virtual ~InterestingList() noexcept = default;

    void set(T n) {
        if (!mList.contains(n))
            mList.append(n);
            std::sort(mList.begin(),mList.end());
    }

    void rem(T n) {
        mList.removeAll(n);
    }

    bool has(T n) const {
        return mList.contains(n);
    }

    void clear() {
        mList.clear();
    }

    size_t count() {
        return mList.size();
    }

    const QList<T> &list() const { return mList; }
};

#endif // INTERESTINGLIST_H
