#ifndef VESSELSSPEC_H
#define VESSELSSPEC_H

#include <string>

#include <memory>
#include <functional>
#include <list>
#include <vector>

class VesselsSpec
{
public:
    VesselsSpec();

    using OnUpdate = std::function<void()>;

    void setOnUpdate(std::weak_ptr<void> owner, OnUpdate onUpdate) {
        mUpdateList.push_back(std::make_pair(owner, onUpdate));
    }

    bool loadFromSpecFile(std::istream &is);

    size_t getCount() const {
        return mVesselsSpec.size();
    }

    struct Record {
        std::string region;
        std::string harbor;
        std::string metier;
        int nVessels;
        int crew;
        float meanLoa;
        float meanGT;
        float meanKw;
        float hakeKg, soleKg, redmulletKg, mantisKg;
        float fishingSpeed,cruiseSpeed, fuelCons;
        float aveStorage,fuelTank,tripDuration,multipFuelSteaming,multipFuelFishing,multipFuelRet,multipFuelInactive,rangeKm;
        float fuelPrice,weekEndStartDay,weekEndEndDay,workHoursStart,workHoursEnd;
    };

    Record getRecord(int i) const {
        return mVesselsSpec.at(i);
    }

    void setRecord (size_t i, const Record &r) {
        if (i < mVesselsSpec.size())
            mVesselsSpec[i] =r ;
    }

private:
    using FuncPair = std::tuple<std::weak_ptr<void>, OnUpdate>;
    std::list<FuncPair> mUpdateList;

    void update();

    using RecordTupleRef = std::tuple<
        std::string &,std::string &,std::string &,
        int &,int &, // crew
        float &,float &,float &, // meanKw
        float &,float &,float &,float &, // mantisKg
        float &,float &,float &,  // fuelCons
        float &,float &,float &,float &,float &,float &,float &,float &,    // rangeKm
        float &,float &,float &,float &,float & // workHoursEnd
    >;
    using RecordTuple = std::tuple<
        std::string,std::string,std::string,
        int,int, // crew
        float,float,float, // meanKw
        float,float,float,float, // mantisKg
        float,float,float,  // fuelCons
        float,float,float,float,float,float,float,float,    // rangeKm
        float,float,float,float,float // workHoursEnd
    >;

    RecordTupleRef recordToTuple (Record &r) const {
        return std::tie(r.region, r.harbor, r.metier,
                        r.nVessels, r.crew,
                        r.meanLoa, r.meanGT, r.meanKw,
                        r.hakeKg, r.soleKg, r.redmulletKg, r.mantisKg,
                        r.fishingSpeed,r.cruiseSpeed, r.fuelCons,
                        r.aveStorage,r.fuelTank,r.tripDuration,r.multipFuelSteaming,r.multipFuelFishing,r.multipFuelRet,r.multipFuelInactive,r.rangeKm,
                        r.fuelPrice,r.weekEndStartDay,r.weekEndEndDay,r.workHoursStart,r.workHoursEnd
                        );
    }

    std::vector<Record> mVesselsSpec;
};

#endif // VESSELSSPEC_H
