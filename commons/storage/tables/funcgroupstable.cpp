#include "funcgroupstable.h"

#include <Node.h>
#include "utils/make_unique.h"

#include "insertstatement.h"

struct FuncGroupsTable::Impl
{
    bool init = false;
    std::mutex mutex;

    sqlite::InsertStatement<
        FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real> > insertStatement;
};


FuncGroupsTable::FuncGroupsTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
: SQLiteTable(db,name), p(utils::make_unique<Impl>())
{
    if (!db->tableExists(name))
        create();
    p->insertStatement = sqlite::makeInsertStatement(fldTStep,
                                                      fldFGroup ,
                                                      fldNodeId ,
                                                      benthosNum ,
                                                      benthosBioTot ,
                                                      benthosMeanWTot ,
                                                      benthosBioK ,
                                                      benthosNumK,
                                                      benthosBioTotK ,
                                                      benthosNumTot ,
                                                      benthosBio ,
                                                      benthosBioMean ,
                                                      benthosNumTotK);

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
                           fldFGroup ,
                           fldNodeId ,
                           benthosNum ,
                           benthosBioTot ,
                           benthosMeanWTot ,
                           benthosBioK ,
                           benthosNumK,
                           benthosBioTotK ,
                           benthosNumTot ,
                           benthosBio ,
                           benthosBioMean ,
                           benthosNumTotK
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
    if(benthos_tot_meanweight.at(funcgr)!=0)
        benthosnumber = benthos_tot_biomass.at(funcgr)/benthos_tot_meanweight.at(funcgr); // #1

    double benthosnumberoverK=0;
    if(!benthos_tot_number_K.empty() && benthos_tot_number_K.at(funcgr)!=0)
        benthosnumberoverK = benthosnumber/benthos_tot_meanweight.at(funcgr);

    double benthosbiomassoverK=0;
    if(!benthos_tot_biomass_K.empty() && benthos_tot_biomass_K.at(funcgr)!=0)
        benthosbiomassoverK = benthos_tot_biomass.at(funcgr)/benthos_tot_biomass_K.at(funcgr);

    double benthosbiomass = benthos_tot_number.at(funcgr)*benthos_tot_meanweight.at(funcgr); // #2

    //double benthosnumberoverK=0;
    if(!benthos_tot_number_K.empty() && benthos_tot_number_K.at(funcgr)!=0)
        benthosnumberoverK = benthos_tot_number.at(funcgr)/benthos_tot_number_K.at(funcgr); // #4

    //double benthosbiomassoverK=0;
    if(!benthos_tot_biomass_K.empty() && benthos_tot_biomass_K.at(funcgr)!=0)
        benthosbiomassoverK = benthosbiomass/benthos_tot_biomass_K.at(funcgr); // #5

   //p->insertStatement.insert(tstep, funcgr, node->get_idx_node().toIndex(),
   //                           benthosnumber, benthos_tot_biomass.at(funcgr),benthos_tot_meanweight.at(funcgr),
   //                           benthosbiomassoverK, benthosnumberoverK, benthos_tot_biomass_K.at(funcgr),
   //                           benthos_tot_number.at(funcgr), benthosbiomass, benthos_tot_meanweight.at(funcgr),
   //                           benthos_tot_number_K.at(funcgr)
                              );
    p->insertStatement.insert(tstep, funcgr, node->get_idx_node().toIndex(),  node->get_marine_landscape(),
                              benthosnumber, benthosbiomass, benthos_tot_meanweight.at(funcgr),
                              benthosbiomassoverK, benthosnumberoverK
                              );

#if 0

    // pop/ tstep / node / long / lat / number func group id /biomass func group id/ mean weight func group id / benthosbiomassoverK / benthosnumberoverK /benthos_tot_biomass_K.at(funcgr)
    if(benthos_tot_biomass.at(funcgr)>1e-6)
        benthosbiomassnodes << funcgr << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " << benthosnumber << " " << benthos_tot_biomass.at(funcgr) << " "  <<
                           benthos_tot_meanweight.at(funcgr) << " " << benthosbiomassoverK  << " " << benthosnumberoverK <<  " " <<  benthos_tot_biomass_K.at(funcgr) << endl;
    if(benthos_tot_number.at(funcgr)>1e-6) benthosnumbernodes << funcgr << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " << benthos_tot_number.at(funcgr) << " " << benthosbiomass << " "  <<
                       benthos_tot_meanweight.at(funcgr) <<  " " << benthosbiomassoverK  << " " << benthosnumberoverK << " " << benthos_tot_number_K.at(funcgr) << endl;
#endif
}
