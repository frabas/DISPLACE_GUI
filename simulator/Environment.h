//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_ENVIRONMENT_H
#define DISPLACE_ENVIRONMENT_H

#include <memory>

#include <env/ActorsContainer.h>
#include <actors/Vessel.h>
#include <env/Clock.h>
#include <env/Calendar.h>

namespace displace {

    namespace env {
        class Playground;
    }

    class Environment {
    private:
        struct Impl;

        std::unique_ptr<Impl> mImpl;
        std::unique_ptr<env::Playground> mPlayground;
        std::unique_ptr<env::Clock> mClock;
        std::unique_ptr<env::Calendar> mCalendar;

        std::unique_ptr<env::ActorsContainer<actors::Vessel>> mVessels;

    public:
        explicit Environment();
        ~Environment() noexcept ;

        env::Clock &clock() { return *mClock; }
        env::Calendar &calendar() { return *mCalendar; }
        env::Playground &playground() { return *mPlayground; }

        env::ActorsContainer<actors::Vessel> &vessels() { return *mVessels; }

        void assignCalendar(std::unique_ptr<env::Calendar> calendar);

    };

}

#endif //DISPLACE_ENVIRONMENT_H
