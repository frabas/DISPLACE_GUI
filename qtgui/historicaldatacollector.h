#ifndef HISTORICALDATACOLLECTOR_H
#define HISTORICALDATACOLLECTOR_H

#include <QMap>
#include <QLinkedList>
#include <memory>

template <typename T>
class HistoricalDataCollector {
public:
    typedef T Value;
    typedef QMap<int, Value> Container;

    HistoricalDataCollector() {
    }

    void insertValue (int tstep, const Value &data) {
        mData.insert(tstep, data);
    }

    const Value &getValue(int tstep) const {
        typename Container::const_iterator it = mData.lowerBound(tstep+1);
        --it;
        return it.value();
    }

    int getUniqueValuesCount() const {
        return mData.size();
    }

    typename Container::const_iterator getFirst() const {
        return mData.begin();
    }

    void clear() { mData.clear(); }

protected:
    Container mData;
};

#endif // HISTORICALDATACOLLECTOR_H
