//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_LOADER_H
#define DISPLACE_LOADER_H

#include <memory>
#include <string>

namespace displace {
    class Simulation;

    namespace io {

        /** @brief A builder class that generate an instance of the right loader for the project file */
        class Loader {
        private:

        public:
            static std::unique_ptr<Loader> create(const std::string &path);

            virtual ~Loader() noexcept = default;
            virtual bool load(Simulation *) = 0;
        };

    }
}



#endif //DISPLACE_LOADER_H
