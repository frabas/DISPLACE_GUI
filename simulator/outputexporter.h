#ifndef OUTPUTEXPORTER_H
#define OUTPUTEXPORTER_H

#include <memory>
#include <fstream>
#include <vector>

class Vessel;
class Population;
class SQLiteOutputStorage;

class OutputExporter
{
    static std::shared_ptr <OutputExporter> mInstance;

    std::string mBasePath;
    std::ofstream mVmsLike;
    std::ofstream mVmsLikeFPingsOnly;
    std::ofstream mLogLike;
    std::ofstream mTripCatchesPerSzgroup;

    bool useSql = true;
    bool usePlainText = true;
    std::shared_ptr<SQLiteOutputStorage> mSqlDb;
public:
    OutputExporter(const std::string &basepath, const std::string &namesimu);

    void exportVmsLike(unsigned int tstep, Vessel *vessel);
    void exportVmsLikeFPingsOnly (unsigned int tstep, Vessel *vessel, const std::vector<Population *> &populations, std::vector<int> &implicit_pops);
    void exportLogLike(unsigned int tstep, Vessel *vessel, const std::vector<Population* >& populations, std::vector<int> &implicit_pops);
    void exportTripCatchPopPerSzgroup(unsigned int tstep, Vessel *vessel, const std::vector<Population* >& populations, std::vector<int> &implicit_pops);

    void close();

    void setUseSqlite(bool sql) { useSql = sql; }
    void setUsePlainText(bool pt) { usePlainText = pt; }
    void setSQLiteDb(std::shared_ptr<SQLiteOutputStorage> db) { mSqlDb = db; }

    /* --- Statics --- */
    static bool instantiate(const std::string &basepath, const std::string &namesimu);
    static OutputExporter &instance() { return *mInstance; }

private:
    void exportLogLikePlaintext(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, std::vector<int> &implicit_pops);
    void exportLogLikeSQLite(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, std::vector<int> &implicit_pops);

    void exportVmsLikePlaintext(unsigned int tstep, Vessel *vessel);
    void exportVmsLikeSQLite(unsigned int tstep, Vessel *vessel);
};

#endif // OUTPUTEXPORTER_H
