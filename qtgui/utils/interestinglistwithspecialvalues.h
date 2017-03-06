#ifndef INTERESTINGLISTWITHSPECIALVALUES_H
#define INTERESTINGLISTWITHSPECIALVALUES_H

#include <utils/interestinglist.h>

template <typename T>
class InterestingListWithSpecialValues : public InterestingList<T> {
    QList<bool> mSpecialValues;
    QList<QString> mSpecialValuesName;
    QString mGroupFormatString;

public:
    InterestingListWithSpecialValues()
        : InterestingList<T>() {
    }

    void setValuesFormatString(QString groupFormatString) {
        mGroupFormatString = groupFormatString;
    }

    void addSpecialValue(QString name, bool selected = false) {
        mSpecialValuesName.push_back(name);
        mSpecialValues.push_back(selected);
    }

    bool isSpecialValueSelected (int idx) const {
        return mSpecialValues[idx];
    }

    void setSpecialValueSelected (int idx, bool selected) {
        mSpecialValues[idx] = selected;
    }

    size_t getSpecialValuesCount() const {
        return mSpecialValues.size();
    }

    QString getSpecialName (int idx) const {
        return mSpecialValuesName[idx];
    }

    QString getFormatString () const {
        return mGroupFormatString;
    }
};

#endif // INTERESTINGLISTWITHSPECIALVALUES_H
