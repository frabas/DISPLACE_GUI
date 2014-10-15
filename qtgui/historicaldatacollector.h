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
        typename Container::const_iterator it = mData.lowerBound(tstep);
        --it;
        return it.value();
    }

//    int getFirstStep() const ;
//    int getLastStep() const;

    int getUniqueValuesCount() const {
        return mData.size();
    }

//    const std::shared_ptr<T> getUniqueValueAt(int n) const;

    typename Container::const_iterator getFirst() const {
        return mData.begin();
    }

protected:
    Container mData;
    int mFirst, mLast;
};

#endif // HISTORICALDATACOLLECTOR_H
