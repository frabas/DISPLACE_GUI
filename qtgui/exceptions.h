#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>
#include <stdexcept>

class DisplaceException : public std::runtime_error {
public:
    explicit DisplaceException (QString msg)
        : runtime_error(msg.toStdString()) {
    }
};

#endif // EXCEPTIONS_H
