//
// Created by happycactus on 02/12/20.
//

#include "PopulationTableDefs.h"

namespace displace {
namespace db {
namespace defs {
const char *PopulationListTableName = "Populations";
const char *PopulationTableName = "PopulationParameters";
const char *PopulationTableNameTrM = "PopulationTransitionMatrix";
const char *PopulationTableNameSzAg = "PopulationParametersWithSizeGroupAndAge";

msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPopId{"id"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSpecies{"pop_id"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldPopName{"name"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldBioSce{"biosce"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldParameter{"parameter"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSzGroup{"size_group"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSzGroup1{"sizegroup1"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSzGroup2{"sizegroup2"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldAge{"age"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldNode{"node"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldLandscape{"landscape"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldCountry{"country"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPeriod{"period"};
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Real> fieldValue{"value"};

}
}
}
