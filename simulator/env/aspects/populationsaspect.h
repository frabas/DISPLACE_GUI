//
// Created by Federico Fuga on 01/04/16.
//

#ifndef DISPLACE_POPULATIONSASPECT_H
#define DISPLACE_POPULATIONSASPECT_H

#include <vector>

namespace displace {
    namespace env {
        namespace node {

            struct PopulationsAspect {
                std::vector<std::vector<float>> population;
            };

        }
    }
}

#endif //DISPLACE_POPULATIONSASPECT_H
