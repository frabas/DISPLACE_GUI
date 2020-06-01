//
// Created by fuga on 01/06/2020.
//

#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Vessel.h"
#include "VesselsLoader.h"

#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace db = msqlitecpp::v2;


void DatabaseModelLoader::loadVessels(int year, int month, int quarter, int semester)
{
    VesselsLoader loader(*p->db);

    auto allVessels = loader.getListOfAllVessels();

    std::vector<Vessel *> vesselsToAdd;
    for (auto &vessel: allVessels) {
        // TODO: in the DB we just have opt1 as "period". We select month here. FIXME.
        auto vesselData = loader.getVesselData(vessel, month);

        // TODO: There's no a_location in the db. FIXME.
        auto v = new Vessel(vessel, nullptr);

        // TODO fill the data here.

        vesselsToAdd.push_back(v);
    }

    model().setVessels(vesselsToAdd);
}

