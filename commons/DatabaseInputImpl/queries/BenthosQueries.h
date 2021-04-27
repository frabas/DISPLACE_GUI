//
// Created by happycactus on 27/04/21.
//

#ifndef DISPLACE_BENTHOSQUERIES_H
#define DISPLACE_BENTHOSQUERIES_H

#include "ModelLoader.h"

#include <msqlitecpp/v2/storage.h>
#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace displace {
namespace db {
namespace defs {
namespace benthos {

extern const char *BenthosTableName;

extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldParameter;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldLandscapeId;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldFuncGroup;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPeriod;
extern msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Real> fieldValue;

}
}
}
}

class BenthosQueries {
    msqlitecpp::v2::Storage &db;

    using func = std::function<void(ModelLoader::BenthosData &benthos, int landscape, int funcgroup,
                                    int period, double value)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    static map dispatcher;

    msqlitecpp::v2::SelectStatement<
            decltype(displace::db::defs::benthos::fieldParameter),
            decltype(displace::db::defs::benthos::fieldLandscapeId),
            decltype(displace::db::defs::benthos::fieldFuncGroup),
            decltype(displace::db::defs::benthos::fieldPeriod),
            decltype(displace::db::defs::benthos::fieldValue)
    > selectQuery;

    void dispatch(ModelLoader::BenthosData &benthos,
                  std::string parameter, int landscape, int funcgroup, int period,
                  double value);

public:
    explicit BenthosQueries(msqlitecpp::v2::Storage &_db, int period = 0);

    void execute(std::vector<ModelLoader::BenthosData> &benthos);
};

#endif //DISPLACE_BENTHOSQUERIES_H
