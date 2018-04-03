#include "shipstable.h"

#include "Ship.h"

#include "insertstatement.h"

#include "sqlitefielddef.h"
using namespace sqlite;

struct ShipsTable::Impl
{
    const FieldDef<FieldType::Integer> fTStep;
    const FieldDef<FieldType::Real> fFueluse;
    const FieldDef<FieldType::Real> fNOxEmission_gperKWh;
    const FieldDef<FieldType::Real> fSOxEmission_percentpertotalfuelmass;
    const FieldDef<FieldType::Real> fGHGEmission_gperKWh;
    const FieldDef<FieldType::Real> fPMEEmission_gperKWh;

    InsertStatement<
        decltype(fTStep),
        decltype(fFueluse),
        decltype(fNOxEmission_gperKWh), decltype(fSOxEmission_percentpertotalfuelmass),
        decltype(fGHGEmission_gperKWh), decltype(fPMEEmission_gperKWh)
        >insertStatement;

    Impl()
        : fTStep("tstep"), fFueluse("fueluse"),
          fNOxEmission_gperKWh("NOx"),
          fSOxEmission_percentpertotalfuelmass("SOx"),
          fGHGEmission_gperKWh("GHG"),
          fPMEEmission_gperKWh("PME"),
          insertStatement()
    {

    }

    void create(SQLiteTable *t) {
        t->create(fTStep, fFueluse, fNOxEmission_gperKWh, fSOxEmission_percentpertotalfuelmass, fGHGEmission_gperKWh, fPMEEmission_gperKWh
                  );
    }
};

ShipsTable::ShipsTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
    p->insertStatement.attach(db, name);
}

ShipsTable::~ShipsTable() noexcept = default;

void ShipsTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    p->create(this);
}

void ShipsTable::exportShipsIndivators(int tstep, Ship *ship)
{
    p->insertStatement.insert(
                tstep, ship->get_fueluse(),
                ship->get_NOxEmission_gperKWh(), ship->get_SOxEmission_percentpertotalfuelmass(),
                ship->get_GHGEmission_gperKWh(), ship->get_PMEEmission_gperKWh()
                );
}



