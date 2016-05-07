#ifndef OUTPUTEXPORTER_H
#define OUTPUTEXPORTER_H

#include <memory>
#include <fstream>
#include <vector>

class Vessel;
class Population;

class OutputExporter
{
    static std::shared_ptr <OutputExporter> mInstance;

    std::string mBasePath;
    std::ofstream mVmsLike;
    std::ofstream mLogLike;
public:
    OutputExporter(const std::string &basepath, const std::string &namesimu);

    void exportVmsLike(unsigned int tstep, Vessel *vessel);
    void exportLogLike(unsigned int tstep, Vessel *vessel, const std::vector<Population* >& populations, std::vector<int> &implicit_pops);

    void close();

    /* --- Statics --- */
    static bool instantiate(const std::string &basepath, const std::string &namesimu);
    static OutputExporter &instance() { return *mInstance; }
};

#endif // OUTPUTEXPORTER_H
