//
// Created by fuga on 26/02/2020.
//

#include "TextfileModelLoader.h"
#include "Impl.h"
#include "myutils.h"
#include "Windmill.h"
#include "SimModel.h"

#include <map>
#include <iostream>
#include <fstream>

using namespace std;

bool TextfileModelLoader::loadWindmillsImpl()
{
    map<int, double> init_size_per_windmill = read_size_per_windmill(
            p->folder_name_parameterization, p->inputfolder);
    cout << "Does the size_per_windmill need a check?" << endl;

    //TODO: extend variables in read_size_per_windmill() e.g. read kWh from files etc.

    std::vector<Windmill *> windmills;

    for (auto iter : init_size_per_windmill) {
        auto wm = new Windmill(iter.first, "here_a_windfarm_name",
                               model().nodes().at(iter.first), iter.second, 1, 500,
                               1); // Caution: type is 1, kW is 500, is_active at 1
        windmills.push_back(wm);
    }

    model().setWindmills(std::move(windmills));

    return true;
}
