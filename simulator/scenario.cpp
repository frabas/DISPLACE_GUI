#include "scenario.h"

#include <dirent.h>

#include <string>
#include <iostream>
#include <fstream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace displace::simulation;

Scenario *Scenario::mInstance = 0;

Scenario::Scenario()
{

}

Scenario *Scenario::instance()
{
    if (mInstance == 0)
        mInstance = new Scenario();
    return mInstance;
}

bool Scenario::loadTimeSeries(std::string path, std::string tsscenario)
{
    std::string suffix = tsscenario + ".dat";

    DIR *dir;
    dirent *dirent;

    int nr = 0;
    dir = opendir(path.c_str());
    if (dir) {
        while ((dirent = readdir(dir))) {
            bool isfile = true;
#ifndef __WIN32
            isfile = dirent->d_type == DT_REG;
#endif
            std::string name = std::string(dirent->d_name);

//            if (isfile && name.length() >= suffix.length() && 0 == name.compare (name.length() - suffix.length(), suffix.length(), suffix)) {
            if (isfile && boost::algorithm::ends_with(name, suffix)) {
                bool res = false;
                try {
                    res = readTsFile (path + "/" + name);
                } catch (std::exception &x) {
                    std::cerr << "Exception thrown form readFile(" << path << "/" << name << ") : " << x.what() << std::endl;
                    res = false;
                }

                if (res) {
                    ++nr;
                } else {
                    std::cerr << "Can't read " << dirent->d_name << std::endl;
                }
            }
        }

        closedir(dir);
    } else {
        std::cerr << "Can't open " << path << std::endl;
        return false;
    }

    return true;
}

bool Scenario::readTsFile (std::string filename) throw (boost::bad_lexical_cast)
{
    std::cout << "@DEBUG: Reading TimeSeries file " << filename << std::endl;

    std::ifstream stream;
    stream.open(filename.c_str(), std::ios_base::in);
    if (stream.fail()) {
        return false;
    }

    int val_line = 0;

    while (!stream.eof()) {
        std::string line;
        std::getline(stream, line);
        if (stream.eof())
            break;

        if (boost::algorithm::starts_with(line, "#Version")) {
        } else if (boost::algorithm::starts_with(line, "#") || boost::algorithm::starts_with(line, "\"")) {
            // ignore
        } else {
            // it's a valid line
            if (val_line <= 2) {
                if (boost::algorithm::starts_with(line, "NA")) {
                    ++val_line;
                    continue;   // skip if NA.
                }
            }

            double v = boost::lexical_cast<double>(line);

            switch (val_line) {
            case 0: // threshold 0
                break;
            case 1:
                break;
            case 2:
                break;
            default:
                break;
            }

            ++val_line;
        }
    }

    return true;
}
