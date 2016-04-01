//
// Created by Federico Fuga on 01/04/16.
//

#ifndef DISPLACE_MODELSTRATEGY_H
#define DISPLACE_MODELSTRATEGY_H

namespace displace {
    class Simulation;

    namespace strategy {

        namespace model {

            class ModelStrategy {
            public:
                virtual ~ModelStrategy() noexcept = default;

                virtual bool updatePopulationModel(Simulation *) = 0;
            };

        }

    }
}

#endif //DISPLACE_MODELSTRATEGY_H
