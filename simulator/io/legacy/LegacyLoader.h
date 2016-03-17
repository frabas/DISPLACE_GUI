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
            std::string mModel;
            std::string mScenario;

        public:
            explicit LegacyLoader(const std::string &path);
            ~LegacyLoader() noexcept ;

            virtual bool load(Simulation *simulation);

        protected:
            bool loadConfigFile();
            bool loadScenarioFile();
            bool loadGraph(Simulation *simulation);
            bool loadVessels(Simulation *simulation, int quarter);
            bool loadCalendar(Simulation *simulation);

            struct Status;
            std::unique_ptr<Status> mStatus;
        };
    }
}

#endif //DISPLACE_LEGACYLOADER_H
