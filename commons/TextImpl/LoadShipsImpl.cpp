//
// Created by fuga on 26/02/2020.
//

#include "Impl.h"
#include "TextfileModelLoader.h"
#include "myutils.h"
#include "Ship.h"
#include "SimModel.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void TextfileModelLoader::loadShips()
{
    cout << "Loading ships features" << "\n";

    // read general ship features (incl. their specific lanes)
    vector<string> shipids;
    vector<double> imos;
    vector<double> yearbuilds;
    vector<string> flags;
    vector<string> types;
    vector<double> typecodes;
    vector<double> loas;
    vector<double> KWs;
    vector<double> breadths;
    vector<double> grosstonnages;
    vector<double> nbunits;
    vector<double> fueluses;
    vector<double> NOxEmission_gperKWhs;
    vector<double> SOxEmission_percentpertotalfuelmasss;
    vector<double> GHGEmissions;
    vector<double> PMEmissions;
    vector<double> vmaxs;
    vector<double> vcruises;
    vector<double> lane_ids;
    if (!read_ships_features(shipids, imos, yearbuilds, flags, types, typecodes,
                             loas, KWs, breadths, grosstonnages, nbunits,
                             fueluses, NOxEmission_gperKWhs, SOxEmission_percentpertotalfuelmasss,
                             GHGEmissions, PMEmissions,
                             vmaxs, vcruises, lane_ids,
                             p->folder_name_parameterization, p->inputfolder)) {
        cerr << "Error loading ship features. Bailing out.\n";
        throw std::runtime_error("Error loading ship features.");
    }

    // read shipping lanes
    multimap<int, double> shiplanes_lat = read_shiplanes_lat(p->folder_name_parameterization,
                                                             p->inputfolder);
    multimap<int, double> shiplanes_lon = read_shiplanes_lon(p->folder_name_parameterization,
                                                             p->inputfolder);

    auto ships = vector<Ship *>(shipids.size());
    for (unsigned int i = 0; i < shipids.size(); i++) {
        cout << "create ship " << shipids.at(i) << "\n";

        vector<double> longs = find_entries_i_d(shiplanes_lon, lane_ids.at(i));
        vector<double> lats = find_entries_i_d(shiplanes_lat, lane_ids.at(i));
        ships[i] = new Ship(i,
                            shipids.at(i),
                            1,
                            imos.at(i),
                            yearbuilds.at(i),
                            flags.at(i),
                            types.at(i),
                            typecodes.at(i),
                            KWs.at(i),
                            loas.at(i),
                            breadths.at(i),
                            grosstonnages.at(i),
                            nbunits.at(i),
                            fueluses.at(i),
                            NOxEmission_gperKWhs.at(i),
                            SOxEmission_percentpertotalfuelmasss.at(i),
                            GHGEmissions.at(i),
                            PMEmissions.at(i),
                            vmaxs.at(i),
                            vcruises.at(i),
                            longs,
                            lats);
        ships[i]->set_idx_ship(i);

        cout << "at (" << ships[i]->get_x() << "," << ships[i]->get_y() << ") " << "\n";

    }
    cout << "Number of ships created: " << ships.size() << "\n";

    model().setShips(std::move(ships));
}

