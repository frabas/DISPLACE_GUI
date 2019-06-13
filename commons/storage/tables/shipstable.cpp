#include "shipstable.h"

#include "Ship.h"

#include "msqlitecpp/v1/insertstatement.h"
#include "msqlitecpp/v1/selectstatement.h"
#include "msqlitecpp/v1/clauses.h"
#include "msqlitecpp/v1/sqlitefieldsop.h"
#include "msqlitecpp/v1/sqlitefielddef.h"

using namespace sqlite;
using namespace displace::plot;

struct ShipsTable::Impl {
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
    > insertStatement;

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

    void create(SQLiteTable *t)
    {
        t->create(fTStep, fShipId, fFueluse_litreperh,
                  fNOxEmission_gperKWh, fSOxEmission_percentpertotalfuelmass, fGHGEmission_gperKWh,
                  fPMEEmission_gperKWh,
                  fFueluse, fNOxEmission, fSOxEmission, fGHGEmission, fPMEEmission
        );
    }
};

void ShipsTable::init()
{
    if (p->initialized) {
        return;
    }

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
    if (db()->tableExists(name())) {
        db()->dropTable(name());
    }

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
            ship->get_fuel_use_litre(), ship->get_NOxEmission(), ship->get_SOxEmission(), ship->get_GHGEmission(),
            ship->get_PMEEmission()
    );
}

TimelineData ShipsTable::getShipsStatData(displace::plot::ShipsStat stattype, AggregationType aggtype, int shipid,
                                          std::vector<int> shiptypeid)
{
    init();
    TimelineData tl;

    bool filterGrpId = (shipid >= 0 && shipid != 999);

    FieldDef<FieldType::Real> f("");
    FieldDef<FieldType::Real> fld("");
    switch (stattype) {
        //case ShipsStat::SH_NbTransportedUnits:
        case ShipsStat::SH_FuelPerHour:
            fld = p->fFueluse_litreperh;
            break;
        case ShipsStat::SH_NOxEmission_gperkW:
            fld = p->fNOxEmission_gperKWh;
            break;
        case ShipsStat::SH_SOxEmission_PercentPerFuelMass:
            fld = p->fSOxEmission_percentpertotalfuelmass;
            break;
        case ShipsStat::SH_GHGEmission_gperkW:
            fld = p->fGHGEmission_gperKWh;
            break;
        case ShipsStat::SH_PMEEmission_gperkW:
            fld = p->fPMEEmission_gperKWh;
            break;
        case ShipsStat::SH_FuelUseLitre:
            fld = p->fFueluse_litreperh;
            break;
        case ShipsStat::SH_NOxEmission:
            fld = p->fNOxEmission;
            break;
        case ShipsStat::SH_SOxEmission:
            fld = p->fSOxEmission;
            break;
        case ShipsStat::SH_GHGEmission:
            fld = p->fGHGEmission;
            break;
        case ShipsStat::SH_PMEEmission:
            fld = p->fPMEEmission;
            break;
        default:
            std::cerr << "*** WARNING: Unhandled case Ships Stat: " << static_cast<int>(stattype) << "\n";
            return tl;
    }

    switch (aggtype) {
        case displace::plot::AggregationType::Avg:
            f = op::avg(fld);
            break;
        case displace::plot::AggregationType::Min:
            f = op::min(fld);
            break;
        case displace::plot::AggregationType::Max:
            f = op::max(fld);
            break;
        case displace::plot::AggregationType::Sum:
            f = op::sum(fld);
            break;
        case displace::plot::AggregationType::None:
            f = fld;
            break;
    }

    auto select = sqlite::statements::Select(name(),
                                             p->fTStep,
                                             f
    );

    /*
    if (shiptypeid.size() > 0) {
        std::ostringstream ss;
        if (filterGrpId)
            ss << p->mFuncGroupsTable->fldFGroup.name() << " == ? AND ";
        ss << p->mFuncGroupsTable->fldBType.name() << " IN (?";
        for (size_t i = 1; i < btype.size() ; ++i)
            ss << ",?";
        ss << ")";
        select.where(ss.str());
    } else {
        if (filterGrpId)
            select.where(op::eq(p->mFuncGroupsTable->fldFGroup));
    }*/

    if (filterGrpId) {
        select.where(op::eq(p->fShipId));
    }

    select.groupBy(p->fTStep);

    //qDebug() << "Ships Select: " << QString::fromStdString(select.string()) << " (" << shipid << "," << shiptypeid<< "): ";

    sqlite::SQLiteStatement stmt(db(), select);

    int n = 1;
    if (filterGrpId) {
        stmt.bind(n++, shipid);
    }
    /*
    for (size_t i = 0; i < btype.size(); ++i)
        stmt.bind(n+i, btype[i]);*/

    stmt.execute([&stmt, &tl]() {
        tl.t.push_back(stmt.getIntValue(0));
        tl.v.push_back(stmt.getDoubleValue(1));
        return true;
    });

    //qDebug() << "Reslut: " << tl.t.size();

    return tl;
}


