#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <qtcommons_global.h>
#include <QList>
#include <QStringList>
#include <QException>

class QTCOMMONSSHARED_EXPORT CsvImporter
{
public:
    class Exception : public QException {
    public:
        explicit Exception (QString what)
            : mWhat(what) {
        }
        ~Exception() noexcept {}

        void raise() const override { throw *this; }
        Exception *clone() const override { return new Exception(*this); }

        const char *what() const noexcept override {
            return mWhat.toStdString().c_str();
        }

    protected:
        QString mWhat;
    };

    CsvImporter();

    QList<QStringList> import (QString filename);

    void setSeparator (QChar sep) { mSeparator = sep; }
protected:
    QChar mSeparator;
    Qt::SplitBehaviorFlags mSplitBehaviour;
};

#endif // CSVIMPORTER_H
