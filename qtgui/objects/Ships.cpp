#include <objects/Ships.h>

#include <readdata.h>

#include <QDebug>

#include <vector>
#include <stdexcept>

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

    string filename = folder.toStdString()+"/shipsspe_"+folder_param.toStdString()+"/shipsspe_features.dat";

    ifstream ships_features;
    ships_features.open(filename.c_str());
    if(ships_features.fail())
    {
        throw std::runtime_error("ship load fail");
    }

    fill_from_ships_specifications(ships_features, shipids, vmaxs, vcruises, lane_ids);
    ships_features.close();

    std::vector<std::string>::iterator it = shipids.begin();
    while (it != shipids.end()) {
        Ship ship(*it);
        res.push_back(ship);

        ++it;
    }

    return res;
}
