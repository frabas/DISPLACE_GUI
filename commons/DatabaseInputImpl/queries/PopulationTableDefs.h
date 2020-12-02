//
// Created by happycactus on 02/12/20.
//

#ifndef DISPLACE_POPULATIONTABLEDEFS_H
#define DISPLACE_POPULATIONTABLEDEFS_H

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace displace {
namespace db {
namespace defs {

extern const char *PopulationListTableName;
extern const char *PopulationTableName;
extern const char *PopulationTableNameTrM;
extern const char *PopulationTableNameSzAg;

extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPopId;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSpecies;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldPopName;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldBioSce;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldParameter;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSzGroup;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSzGroup1;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSzGroup2;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldAge;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldNode;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldLandscape;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldCountry;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPeriod;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Real> fieldValue;

}
}
}

#endif //DISPLACE_POPULATIONTABLEDEFS_H
