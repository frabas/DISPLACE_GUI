//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_SIMULATION_H
#define DISPLACE_SIMULATION_H

#include <list>
#include <string>
#include "Environment.h"
#include <ipc/outputqueuemanager.h>

namespace displace {

    class Environment;

    class Simulation {
    private:
        struct Impl;
        std::unique_ptr<Impl> mImpl;

        std::unique_ptr<Environment> mEnv;
    public:
        explicit Simulation();
        ~Simulation() noexcept ;

        bool initialize(const std::list<std::string> &options);
        int run();

        Environment &environment() { return *mEnv; }
        const Environment &environment() const { return *mEnv; }

    private:
        void applyPopulationModel();
        void applyManagementModel();
        void applyVesselsModel();

        OutputQueueManager mOutQueueManager;
    };

} // ns;

#endif //DISPLACE_SIMULATION_H
