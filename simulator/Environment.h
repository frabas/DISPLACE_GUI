//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_ENVIRONMENT_H
#define DISPLACE_ENVIRONMENT_H

#include <memory>

#include <env/ActorsContainer.h>
#include <actors/Vessel.h>

namespace displace {

    namespace env {
        class Playground;
    }

    class Environment {
    private:
        struct Impl;

        std::unique_ptr<Impl> mImpl;
        std::unique_ptr<env::Playground> mPlayground;

        std::unique_ptr<env::ActorsContainer<actors::Vessel>> mVessels;

    public:
        explicit Environment();
        ~Environment() noexcept ;

        env::Playground &playground() { return *mPlayground; }

        env::ActorsContainer<actors::Vessel> &vessels() { return *mVessels; }
    };

}

#endif //DISPLACE_ENVIRONMENT_H
