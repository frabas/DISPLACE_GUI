//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_ENVIRONMENT_H
#define DISPLACE_ENVIRONMENT_H

#include <memory>

namespace displace {

    namespace env {
        class Playground;
    }

    class Environment {
    private:
        struct Impl;
        std::unique_ptr<Impl> mImpl;

        std::unique_ptr<env::Playground> mPlayground;
    public:
        explicit Environment();
        ~Environment() noexcept ;

        env::Playground &playground() { return *mPlayground; }
    };

}

#endif //DISPLACE_ENVIRONMENT_H
