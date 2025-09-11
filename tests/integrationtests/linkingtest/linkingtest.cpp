#include <iostream>
#include <boost/program_options.hpp>

int main(int argc, char* argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Show this help")
        ("version,V", "Show the version number and exit");

    try {
        po::variables_map vm;
        auto parsed = po::command_line_parser(argc, argv)
                          .options(desc)
                          .run();
        po::store(parsed, vm);
        po::notify(vm);

        if (vm.count("version") && vm.size() == 1) {
            std::cout << "displace 1.0.0\n";
            return 0;
        }

        if (vm.count("help") && vm.size() == 1) {
            std::cout << desc << "\n";
            return 0;
        }

        return 1;
    } catch (const po::error& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}