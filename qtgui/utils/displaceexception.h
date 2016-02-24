#ifndef DISPLACEEXCEPTION_H
#define DISPLACEEXCEPTION_H

#include <QException>
#include <QString>

namespace displace {

class DisplaceException : public QException {
public:
    explicit DisplaceException (QString what, QString file = QString(), int line = -1)
        : mWhat(what), mFile(file), mLine(line) {
    }

    void raise() const { throw *this; }
    DisplaceException *clone() const { return new DisplaceException(*this); }

    QString message() const {
        return mWhat;
    }
    const char *what() const _NOEXCEPT {
        return mWhat.toStdString().c_str();
    }
    QString file() const {
         return mFile;
    }
    int line() const {
        return mLine;
    }
protected:
    QString mWhat;
    QString mFile;
    int mLine;
};

}

#endif // DISPLACEEXCEPTION_H
