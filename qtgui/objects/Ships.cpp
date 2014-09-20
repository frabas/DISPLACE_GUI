#include <objects/Ships.h>

#include <readdata.h>
#include <vector>

Ship::Ship(const std::string &name)
    : mName(name.c_str())
{

}

Ship::~Ship()
{

}

QList<Ship> Ship::readFromFile(QString folder_param, QString folder)
{
    QList<Ship> res;
    std::vector<std::string> shipids;
    std::vector<double> vmaxs;
    std::vector<double> vcruises;
    std::vector<double> lane_ids;

    read_ships_features(shipids, vmaxs, vcruises, lane_ids, folder_param.toStdString(), folder.toStdString());

    std::vector<std::string>::iterator it = shipids.begin();
    while (it != shipids.end()) {
        Ship ship(*it);
        res.push_back(ship);

        ++it;
    }

    return res;
}
