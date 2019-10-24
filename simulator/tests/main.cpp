//
// Created by happycactus on 07/08/19.
//

#include "LoaderTestSuite.h"

#include <msqlitecpp/v2/storage.h>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

namespace po = boost::program_options;
namespace db = msqlitecpp::v2;

int main(int argc, char *argv[])
{
    std::string dbname;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "Show Help")
            ("indb,i", po::value(&dbname), "Database to load");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    if (!boost::filesystem::exists(dbname) || !boost::filesystem::is_regular_file(dbname)) {
        std::cerr << "Db: " << dbname << " doesn't exist or is not a file\n";
        return 1;
    }

    try {
        db::Storage db(dbname);

        LoaderTestSuite testSuite(db);
        testSuite.prepare();
        testSuite.loadNodes();
        testSuite.loadEdges();
    } catch (std::exception &x) {
        std::cerr << "Error: " << x.what() << "\n";
        return 2;
    }
    return 0;
}


