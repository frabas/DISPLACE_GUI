#include "vesseldeftable.h"

#include <sqlitestatement.h>
#include <sqlitestatementformatters.h>

#include <iterator>

using namespace sqlite;

struct VesselDefTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Text>,FieldDef<FieldType::Text>,
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

void VesselDefTable::feedVesselsDefTable(const std::vector<std::string> &vesselids, const std::vector<double> &speeds, const std::vector<double> &length)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldName,
                                                     fldNationality,
                                                     fldSpeeds,
                                                     fldLengths
                                                     ));
    }

    auto l = vesselids.size();
    for (size_t i = 0; i < l; ++i) {
        std::string nat;
        std::copy(vesselids[i].begin(), vesselids[i].begin() + 3, std::back_inserter(nat));
        insert (p->statement, std::make_tuple(vesselids[i],
                nat, speeds[i],length[i]));
    }
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

void VesselDefTable::createIndex()
{
    SQLiteTable::createIndex("NationalityIndex", std::make_tuple(fldNationality));
}


