//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_CLOCK_H
#define DISPLACE_CLOCK_H

namespace displace {
    namespace env {

        class Clock {
        public:
            Clock() {
            }

            unsigned long timestep () const { return mTimeStep; }
            void advance() { ++mTimeStep; }

        private:
            unsigned long mTimeStep = 0;
        };

    }
}


#endif //DISPLACE_CLOCK_H
