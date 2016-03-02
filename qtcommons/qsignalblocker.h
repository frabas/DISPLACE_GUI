#ifndef QSIGNALBLOCKER_H
#define QSIGNALBLOCKER_H

#include <QObject>

#include <QtGlobal>
#if QT_VERSION >= 0x050300
#include <QSignalBlocker>
#else
class QSignalBlocker {
    QObject *obj;
    bool mWasBlocked;

public:
    explicit QSignalBlocker(QObject *object)
        : obj(object), mWasBlocked(obj!= nullptr && obj->signalsBlocked()) {
        if (obj != nullptr)
            obj->blockSignals(true);
    }

    ~QSignalBlocker() {
        if (obj != nullptr)
            obj->blockSignals(mWasBlocked);
    }
};

#endif

#endif // QSIGNALBLOCKER_H
