#ifndef POPQUOTASTABLE_H
#define POPQUOTASTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Population;

class COMMONSSHARED_EXPORT PopQuotasTable : public SQLiteTable
{
private:
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldGroup = makeFieldDef("PopGroup",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> fldQuotasUptake = makeFieldDef("QuotasUptake",FieldType::Real());
    const FieldDef<FieldType::Real> fldQuotas = makeFieldDef("Quotas",FieldType::Real());

    PopQuotasTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~PopQuotasTable() noexcept;

    void dropAndCreate();
    void insert (int tstep, int popid, Population *pop);
};

#endif // POPQUOTASTABLE_H
