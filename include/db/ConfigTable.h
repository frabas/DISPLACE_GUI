//
// Created by happycactus on 05/08/19.
//

#ifndef DISPLACE_CONFIGTABLE_H
#define DISPLACE_CONFIGTABLE_H

#include "commons_global.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/selectstatement.h"

#include "idtypes.h"
#include "idtypeshelpers.h"
#include "utils/vectorsdata.h"

#include <boost/lexical_cast.hpp>

#include <map>

namespace db = msqlitecpp::v2;

namespace displace {
namespace in {

class ConfigTable {
    static COMMONSSHARED_EXPORT const char *const TableName;

    db::Column<db::ColumnTypes::Text> Param{"param"};
    db::Column<db::ColumnTypes::Text> Value{"value"};

    std::map<std::string, std::string> mParams;

    template<typename T>
    T get(std::string v) const
    {
        try {
            return boost::lexical_cast<T>(mParams.at(v));
        } catch (std::out_of_range &x) {
            std::ostringstream ss;
            ss << "Field not found: " << v;
            throw std::out_of_range(ss.str());
        }
    }

    template<typename T>
    T getOptional(std::string v) const
    {
        try {
            return boost::lexical_cast<T>(mParams.at(v));
        } catch (std::out_of_range &x) {
            return T();
        }
    }

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

    auto getNbPops() const
    {
        return get<int>("nbpops");
    }

    auto getNbBenthosPops() const
    {
        return get<int>("nbbenthospops");
    }

    auto getImplicitStocks() const
    {
        return getVector<int>("implicit_stocks");
    }

    auto getCalibLandingsStock() const
    {
        return getVector<double>("calib_landings_stock");
    }

    auto getCalibW() const
    {
        return getVector<double>("calib_w_at_szgroup_stock");
    }

    auto getCalibCpue() const
    {
        return getVector<double>("calib_cpue_stock");
    }

    auto getInterestingArbours() const
    {
        return types::helpers::toIdVector<types::NodeId>(getVector<double>("interesting_harbours"));
    }

    auto getImplicitPopLevels2() const
    {
        return getVectorOptional<int>("implicit_pops_level2");
    }

    auto getGroupedTacs() const
    {
        return getVectorOptional<int>("grouped_tacs");
    }

    auto getNbCouplingPops() const
    {
        return getVectorOptional<int>("nbcp_coupling_pops");
    }
};

}
}

#endif //DISPLACE_CONFIGTABLE_H
