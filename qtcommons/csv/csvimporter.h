#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <QList>
#include <QStringList>
#include <QException>

class CsvImporter
{
public:
    class Exception : public QException {
    public:
        explicit Exception (QString what)
            : mWhat(what) {
        }

        void raise() const { throw *this; }
        Exception *clone() const { return new Exception(*this); }

    protected:
        QString mWhat;
    };

    CsvImporter();

    QList<QStringList> import (QString filename);

    void setSeparator (QChar sep) { mSeparator = sep; }
protected:
    QChar mSeparator;
    QString::SplitBehavior mSplitBehaviour;
};

#endif // CSVIMPORTER_H
