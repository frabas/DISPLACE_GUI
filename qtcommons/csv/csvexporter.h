#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <QException>
#include <QString>
#include <QStringList>

class CsvExporter
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

        const char *what() const noexcept {
            return mWhat.toStdString().c_str();
        }

    protected:
        QString mWhat;
    };

    CsvExporter();

    bool exportFile(QString filename, const QList<QStringList> & list) throw (Exception);
    void setSeparator (QChar sep) { mSeparator = sep; }

private:
    QChar mSeparator;
};

#endif // CSVEXPORTER_H
