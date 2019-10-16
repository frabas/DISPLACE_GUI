//
// Created by happycactus on 06/08/19.
//

#ifndef DISPLACE_SCENARIOCONFIGTABLE_H
#define DISPLACE_SCENARIOCONFIGTABLE_H

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/selectstatement.h"

#include "utils/vectorsdata.h"

#include <boost/lexical_cast.hpp>
#include <map>
#include <string>

namespace db = msqlitecpp::v2;

namespace displace {
namespace in {
class ScenarioConfigTable {
    static const char *const TableName;

    db::Column<db::ColumnTypes::Text> Scenario{"sce"};
    db::Column<db::ColumnTypes::Text> Param{"param"};
    db::Column<db::ColumnTypes::Text> Value{"value"};

    std::map<std::string, std::string> mParams;


    template<typename T>
    std::vector<T> getVector(std::string v) const
    {
        try {
            return displace::formats::utils::stringToVector<T>(mParams.at(v));
        } catch (std::out_of_range &x) {
            std::ostringstream ss;
            ss << "Field not found: " << v;
            throw std::out_of_range(ss.str());
        }
    }

    template<typename T>
    std::vector<T> getVectorOptional(std::string v) const
    {
        try {
            return displace::formats::utils::stringToVector<T>(mParams.at(v));
        } catch (std::out_of_range &) {
            return std::vector<T>();
        }
    }

public:
    bool query(db::Storage &db)
    {
        auto select = db::makeSelectStatement(db, TableName, Param, Value);

        select.execute([this](std::string field, std::string value) {
            mParams[field] = value;
            return true;
        });
		return true;
    }

    std::string get(std::string const &field, std::string defaultValue = std::string())
    {
        try {
            return mParams.at(field);
        } catch (std::out_of_range &x) {
            return defaultValue;
        }
    }

    template<typename T>
    T getAs(std::string v, T defaultValue = T()) const
    {
        try {
            return boost::lexical_cast<T>(mParams.at(v));
        } catch (std::out_of_range &x) {
            return defaultValue;
        } catch (boost::bad_lexical_cast &x) {
            std::cout << "WARNING: Bad Lexical Cast converting field: " << v << "\n";
            return defaultValue;
        }
    }
};

}
}

#endif //DISPLACE_SCENARIOCONFIGTABLE_H
