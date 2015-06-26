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
        ~Exception() throw () {}

        void raise() const { throw *this; }
        Exception *clone() const { return new Exception(*this); }

        const char *what() const _GLIBCXX_USE_NOEXCEPT {
            return mWhat.toStdString().c_str();
        }

    protected:
        QString mWhat;
    };

    CsvImporter();

    QList<QStringList> import (QString filename) throw (Exception);

    void setSeparator (QChar sep) { mSeparator = sep; }
protected:
    QChar mSeparator;
    QString::SplitBehavior mSplitBehaviour;
};

#endif // CSVIMPORTER_H
