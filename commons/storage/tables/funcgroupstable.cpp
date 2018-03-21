#include "funcgroupstable.h"

#include <Node.h>
#include "utils/make_unique.h"

#include "insertstatement.h"

struct FuncGroupsTable::Impl
{
    bool init = false;
    std::mutex mutex;

    sqlite::InsertStatement<
        FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Integer>,FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>> insertStatement;
};


FuncGroupsTable::FuncGroupsTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
: SQLiteTable(db,name), p(utils::make_unique<Impl>())
{
    if (!db->tableExists(name))
        create();
    p->insertStatement = sqlite::makeInsertStatement(fldTStep,
                                                     fldFGroup ,
                                                     fldNodeId,
                                                     fldBType,
                                                     benthosNumTot ,
                                                     benthosBio ,
                                                     benthosBioMean,
                                                     benthosBioK ,
                                                     benthosNumK);

    p->insertStatement.attach(db, name);
}

FuncGroupsTable::~FuncGroupsTable() noexcept = default;

void FuncGroupsTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());
    create();
}

void FuncGroupsTable::create()
{
    SQLiteTable::create(std::make_tuple(fldTStep,
                                        fldFGroup,
                                        fldNodeId,
                                        fldBType,
                                        benthosNumTot,
                                        benthosBio ,
                                        benthosBioMean ,
                                        benthosBioK ,
                                        benthosNumK
                           ));
}

void FuncGroupsTable::insert (int tstep, Node *node, int funcgr)
{
    const auto &benthos_tot_number_K = node->get_benthos_tot_number_K();
    const auto &benthos_tot_number = node->get_benthos_tot_number();
    const auto &benthos_tot_biomass = node->get_benthos_tot_biomass();
    const auto &benthos_tot_meanweight = node->get_benthos_meanweight_per_funcgr(); // #3
    const auto &benthos_tot_biomass_K = node->get_benthos_tot_biomass_K();




    double benthosnumber=0;
    double benthosbiomass =0;
    if(benthos_tot_meanweight.at(funcgr)!=0 && benthos_tot_biomass.at(funcgr)!=0){
        benthosnumber = benthos_tot_biomass.at(funcgr)/benthos_tot_meanweight.at(funcgr); // #1
        benthosbiomass = benthos_tot_biomass.at(funcgr); // #2
    }

    if(benthos_tot_meanweight.at(funcgr)!=0 && benthos_tot_number.at(funcgr)!=0){
        benthosnumber = benthos_tot_number.at(funcgr); // #1
        benthosbiomass = benthos_tot_number.at(funcgr)*benthos_tot_meanweight.at(funcgr); // #2
    }

    double benthosnumberoverK=0;
    if(!benthos_tot_number_K.empty() && benthos_tot_number_K.at(funcgr)!=0)
        benthosnumberoverK = benthosnumber/benthos_tot_number_K.at(funcgr);

    double benthosbiomassoverK=0;
    if(!benthos_tot_biomass_K.empty() && benthos_tot_biomass_K.at(funcgr)!=0)
        benthosbiomassoverK = benthosbiomass/benthos_tot_biomass_K.at(funcgr);


     if(benthosbiomass>1e-6) p->insertStatement.insert(tstep, funcgr, node->get_idx_node().toIndex(),  node->get_marine_landscape(),
                              benthosnumber, benthosbiomass, benthos_tot_meanweight.at(funcgr),
                              benthosbiomassoverK, benthosnumberoverK
                              );
}
