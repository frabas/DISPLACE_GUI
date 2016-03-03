//
// Created by Federico Fuga on 03/03/16.
//

#ifndef DISPLACE_LEGACYLOADER_H
#define DISPLACE_LEGACYLOADER_H

#include <io/Loader.h>
#include <boost/filesystem/path.hpp>

namespace displace {
    namespace io {

        class LegacyLoader : public Loader {
            boost::filesystem::path mPath;
        public:
            explicit LegacyLoader(const std::string &path);

            virtual bool load(Simulation *simulation);
        };

    }
}

#endif //DISPLACE_LEGACYLOADER_H
