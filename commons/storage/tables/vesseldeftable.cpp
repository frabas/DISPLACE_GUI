#include "vesseldeftable.h"

#include <Vessel.h>
#include <Node.h>

#include <sqlitestatement.h>
#include <sqlitestatementformatters.h>

#include <iterator>

#include <boost/algorithm/string.hpp>

using namespace sqlite;

struct VesselDefTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Text>,
        FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Text>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>> statement;
};

VesselDefTable::VesselDefTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

VesselDefTable::~VesselDefTable() noexcept = default;

void VesselDefTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldId,
                           fldNode,
                           fldName,
                           fldNationality,
                           fldSpeeds,
                           fldFuelcons,
                           fldLengths,
                           fldVKWs,
                           fldCarrycapacities,
                           fldTankcapacities,
                           fldNbfpingspertrips,
                           fldResttime_par1s,
                           fldResttime_par2s,
                           fldAv_trip_duration,
                           fldMult_fuelcons_when_steaming,
                           fldMult_fuelcons_when_fishing,
                           fldMult_fuelcons_when_returning,
                           fldMult_fuelcons_when_inactive,
                           fldFirm_ids));
}

void VesselDefTable::feedVesselsDefTable(Vessel *vessel)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldId,
                                                     fldName,
                                                     fldNode,
                                                     fldNationality,
                                                     fldSpeeds,
                                                     fldLengths
                                                     ));
    }


    std::string nat = vessel->get_name().substr(0, 3);
    insert (p->statement, std::make_tuple(
                vessel->get_idx(),
                vessel->get_name(),
                int(vessel->get_loc()->get_idx_node().toIndex()),
                nat,
                vessel->get_speed(),
                vessel->get_length()
                ));
}

std::vector<std::string> VesselDefTable::getNationsList()
{
    auto select = sqlite::statements::Select(name(), fldNationality).distinct();

    sqlite::SQLiteStatement stmt(db(),select);

    std::vector<std::string> data;
    stmt.execute([&stmt, &data](){
        data.push_back(stmt.getStringValue(0));
        return true;
    });

    return data;
}

void VesselDefTable::queryAllVessels(
        std::function<Node *(int id)> getnode,
        std::function<bool (std::shared_ptr<Vessel>)> op)
{
    auto select = sqlite::statements::Select(name(),
                                             fldId, fldNode,
                                             fldName, fldSpeeds, fldLengths);
    sqlite::SQLiteStatement stmt(db(), select);
    stmt.execute([&stmt, &op, &getnode] {
        auto id = stmt.getIntValue(1);
        auto node = getnode(id);
        auto d = std::make_shared<Vessel>(node, stmt.getIntValue(0), stmt.getStringValue(2));
        if (op)
            return op(d);
        return false;
    });
}

void VesselDefTable::createIndex()
{
    SQLiteTable::createIndex("NationalityIndex", std::make_tuple(fldNationality));
}


