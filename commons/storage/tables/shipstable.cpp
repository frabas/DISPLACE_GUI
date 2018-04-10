#include "shipstable.h"

#include "Ship.h"

#include "insertstatement.h"
#include <selectstatement.h>
#include <clauses.h>
#include "sqlitefieldsop.h"
#include "sqlitefielddef.h"

using namespace sqlite;

struct ShipsTable::Impl
{
    bool initialized = false;

    FieldDef<FieldType::Integer> fTStep;
    FieldDef<FieldType::Integer> fShipId;
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
        decltype(fTStep), decltype(fShipId),
        decltype(fFueluse_litreperh),
        decltype(fNOxEmission_gperKWh), decltype(fSOxEmission_percentpertotalfuelmass),
        decltype(fGHGEmission_gperKWh), decltype(fPMEEmission_gperKWh),
        decltype(fFueluse),
        decltype(fNOxEmission), decltype(fSOxEmission), decltype(fGHGEmission), decltype(fPMEEmission)
        >insertStatement;

    SelectStatement<
        decltype(fTStep), decltype(fShipId),
        decltype(fFueluse_litreperh), decltype(fNOxEmission_gperKWh), decltype(fSOxEmission_percentpertotalfuelmass),
        decltype(fGHGEmission_gperKWh), decltype(fPMEEmission_gperKWh), decltype(fFueluse),
        decltype(fNOxEmission), decltype(fSOxEmission), decltype(fGHGEmission), decltype(fPMEEmission), decltype(fTStep)>
        allShipsQuery;
    Where<decltype(fTStep)> where;

    // TODO: Add a constrain on the table, to make tstep and ShipId unique.
    Impl()
        : fTStep("tstep"), fShipId("ShipId"), fFueluse_litreperh("FuelUsePerH"),
          fNOxEmission_gperKWh("NOxPerKWh"),
          fSOxEmission_percentpertotalfuelmass("SOxPercent"),
          fGHGEmission_gperKWh("GHGPerKWh"),
          fPMEEmission_gperKWh("PMEPerKWh"),
          fFueluse("FuelUse"),
          fNOxEmission("NOx"),
          fSOxEmission("SOx"),
          fGHGEmission("GHG"),
          fPMEEmission("PME"),
          insertStatement(fTStep, fShipId, fFueluse_litreperh, fNOxEmission_gperKWh,
                          fSOxEmission_percentpertotalfuelmass,
                          fGHGEmission_gperKWh, fPMEEmission_gperKWh,
                          fFueluse, fNOxEmission, fSOxEmission, fGHGEmission, fPMEEmission),
          allShipsQuery(fTStep, fShipId, fFueluse_litreperh, fNOxEmission_gperKWh,
                        fSOxEmission_percentpertotalfuelmass,
                        fGHGEmission_gperKWh, fPMEEmission_gperKWh,
                        fFueluse, fNOxEmission, fSOxEmission, fGHGEmission, fPMEEmission, op::max(fTStep))
    {

    }

    void create(SQLiteTable *t) {
        t->create(fTStep, fShipId, fFueluse_litreperh,
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
    p->allShipsQuery.attach(db(), name());
    p->where.attach(p->allShipsQuery.getStatement(), op::le(p->fTStep));
    p->allShipsQuery.where(p->where);
    p->allShipsQuery.groupBy(p->fTStep);
    p->allShipsQuery.prepare();

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
                tstep, ship->get_idx(), ship->get_fueluse(),
                ship->get_NOxEmission_gperKWh(), ship->get_SOxEmission_percentpertotalfuelmass(),
                ship->get_GHGEmission_gperKWh(), ship->get_PMEEmission_gperKWh(),
                ship->get_fuel_use_litre(), ship->get_NOxEmission(), ship->get_SOxEmission(), ship->get_GHGEmission(), ship->get_PMEEmission()
                );
}

TimelineData ShipsTable::getShipsStatData(displace::plot::ShipsStat stattype)
{
    init();
    TimelineData tl;

    p->allShipsQuery.exec([&stattype,&tl](int tstep, int shipId, double fFueluse_litreperh, double fNOxEmission_gperKWh,
                          double fSOxEmission_percentpertotalfuelmass, double fGHGEmission_gperKWh, double fPMEEmission_gperKWh,
                          double fFueluse, double fNOxEmission, double fSOxEmission, double fGHGEmission, double fPMEEmission, int maxtstep){

        using SH = displace::plot::ShipsStat;

        switch (stattype) {
        case SH::SH_NbTransportedUnits:
            throw std::logic_error("Missing field 'SH_NbTransportedUnits' in table.");
            break;
        case SH::SH_FuelPerHour:
            tl.v.push_back(fFueluse_litreperh); break;
        case SH::SH_NOxEmission_gperkW:
            tl.v.push_back(fNOxEmission_gperKWh); break;
        case SH::SH_SOxEmission_PercentPerFuelMass:
            tl.v.push_back(fSOxEmission_percentpertotalfuelmass); break;
        case SH::SH_GHGEmission_gperkW:
            tl.v.push_back(fGHGEmission_gperKWh); break;
        case SH::SH_PMEEmission_gperkW:
            tl.v.push_back(fPMEEmission_gperKWh); break;
        case SH::SH_FuelUseLitre:
            tl.v.push_back(fFueluse); break;
        case SH::SH_NOxEmission:
            tl.v.push_back(fNOxEmission); break;
        case SH::SH_SOxEmission:
            tl.v.push_back(fSOxEmission); break;
        case SH::SH_GHGEmission:
            tl.v.push_back(fGHGEmission); break;
        case SH::SH_PMEEmission:
            tl.v.push_back(fPMEEmission); break;
        }
        tl.t.push_back(tstep);
        return true;
    });

    return tl;
}


