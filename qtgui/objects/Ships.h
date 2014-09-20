#ifndef OBJECTS_SHIPS_H
#define OBJECTS_SHIPS_H

#include <QString>
#include <QList>

class Ship {
private:
    QString mName;

public:
    Ship(const std::string &name);
    ~Ship();

    // getter
    QString name() const { return mName; }

    static QList<Ship> readFromFile(QString folder_param, QString folder);
};

#endif
