#ifndef OUTPUTEXPORTER_H
#define OUTPUTEXPORTER_H

#include <memory>
#include <fstream>

class Vessel;

class OutputExporter
{
    static std::shared_ptr <OutputExporter> mInstance;

    std::string mBasePath;
    std::ofstream mVmsLike;
public:
    OutputExporter(const std::string &basepath, const std::string &namesimu);

    void exportVmsLike(unsigned int tstep, Vessel *vessel);

    void close();

    /* --- Statics --- */
    static bool instantiate(const std::string &basepath, const std::string &namesimu);
    static OutputExporter &instance() { return *mInstance; }
};

#endif // OUTPUTEXPORTER_H
