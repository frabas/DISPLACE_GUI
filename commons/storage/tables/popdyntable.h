#ifndef POPDYNTABLE_H
#define POPDYNTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Population;

class COMMONSSHARED_EXPORT PopDynTable : public SQLiteTable
{
private:
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldGroup = makeFieldDef("PopGroup",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> fldNz = makeFieldDef("Nz",FieldType::Real());
    const FieldDef<FieldType::Real> fldC = makeFieldDef("Catch",FieldType::Real());
    const FieldDef<FieldType::Real> fldD = makeFieldDef("Discard",FieldType::Real());
    const FieldDef<FieldType::Real> fldravF = makeFieldDef("RunningF",FieldType::Real());
    const FieldDef<FieldType::Real> fldF = makeFieldDef("F",FieldType::Real());
    const FieldDef<FieldType::Real> fldM = makeFieldDef("M",FieldType::Real());
    const FieldDef<FieldType::Real> fldNa = makeFieldDef("Na",FieldType::Real());
    const FieldDef<FieldType::Real> fldW = makeFieldDef("W",FieldType::Real());
    const FieldDef<FieldType::Real> fldMat = makeFieldDef("Mat",FieldType::Real());
    const FieldDef<FieldType::Real> fldSSB = makeFieldDef("SSB",FieldType::Real());
    const FieldDef<FieldType::Real> fldFFmsy = makeFieldDef("FFmsy",FieldType::Real());
    const FieldDef<FieldType::Real> fldPropMature = makeFieldDef("PropMature",FieldType::Real());


    PopDynTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~PopDynTable() noexcept;

    void dropAndCreate();
    void insert (int tstep, int popid, Population *pop);
};

#endif // POPDYNTABLE_H
