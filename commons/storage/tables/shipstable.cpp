#include "shipstable.h"

#include "Ship.h"

#include "insertstatement.h"

#include "sqlitefielddef.h"
using namespace sqlite;

struct ShipsTable::Impl
{
    bool initialized = false;

    FieldDef<FieldType::Integer> fTStep;
    FieldDef<FieldType::Real> fFueluse_litreperh;
    FieldDef<FieldType::Real> fNOxEmission_gperKWh;
    FieldDef<FieldType::Real> fSOxEmission_percentpertotalfuelmass;
    FieldDef<FieldType::Real> fGHGEmission_gperKWh;
    FieldDef<FieldType::Real> fPMEEmission_gperKWh;
    FieldDef<FieldType::Real> fFueluse;
    FieldDef<FieldType::Real> fNOxEmission;
    FieldDef<FieldType::Real> fSOxEmission;
    FieldDef<FieldType::Real> fGHGEmission;
    FieldDef<FieldType::Real> fPMEEmission;


    InsertStatement<
        decltype(fTStep),
        decltype(fFueluse_litreperh),
        decltype(fNOxEmission_gperKWh), decltype(fSOxEmission_percentpertotalfuelmass),
        decltype(fGHGEmission_gperKWh), decltype(fPMEEmission_gperKWh),
        decltype(fFueluse),
        decltype(fNOxEmission), decltype(fSOxEmission), decltype(fGHGEmission), decltype(fPMEEmission)
        >insertStatement;

    Impl()
        : fTStep("tstep"), fFueluse_litreperh("FuelUsePerH"),
          fNOxEmission_gperKWh("NOxPerKWh"),
          fSOxEmission_percentpertotalfuelmass("SOxPercent"),
          fGHGEmission_gperKWh("GHGPerKWh"),
          fPMEEmission_gperKWh("PMEPerKWh"),
          fFueluse("FuelUse"),
          fNOxEmission("NOx"),
          fSOxEmission("SOx"),
          fGHGEmission("GHG"),
          fPMEEmission("PME"),
          insertStatement(fTStep, fFueluse_litreperh, fNOxEmission_gperKWh,
                          fSOxEmission_percentpertotalfuelmass,
                          fGHGEmission_gperKWh, fPMEEmission_gperKWh,
                          fFueluse, fNOxEmission, fSOxEmission, fGHGEmission, fPMEEmission)
    {

    }

    void create(SQLiteTable *t) {
        t->create(fTStep, fFueluse_litreperh,
                  fNOxEmission_gperKWh, fSOxEmission_percentpertotalfuelmass, fGHGEmission_gperKWh, fPMEEmission_gperKWh,
                  fFueluse, fNOxEmission, fSOxEmission, fGHGEmission, fPMEEmission
                  );
    }
};

void ShipsTable::init()
{
    if (p->initialized)
        return;

    p->insertStatement.attach(db(), name());

    p->initialized = true;
}

ShipsTable::ShipsTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

ShipsTable::~ShipsTable() noexcept = default;

void ShipsTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    p->create(this);
    init();
}

void ShipsTable::exportShipsIndivators(int tstep, Ship *ship)
{
    init();
    p->insertStatement.insert(
                tstep, ship->get_fueluse(),
                ship->get_NOxEmission_gperKWh(), ship->get_SOxEmission_percentpertotalfuelmass(),
                ship->get_GHGEmission_gperKWh(), ship->get_PMEEmission_gperKWh(),
                ship->get_fuel_use_litre(), ship->get_NOxEmission(), ship->get_SOxEmission(), ship->get_GHGEmission(), ship->get_PMEEmission()
                );
}

TimelineData ShipsTable::getShipsStatData(displace::plot::ShipsStat stattype)
{
    TimelineData tl;

    return tl;
}


