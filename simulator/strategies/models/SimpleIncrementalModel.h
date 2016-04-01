//
// Created by Federico Fuga on 01/04/16.
//

#ifndef DISPLACE_SIMPLEINCREMENTALMODEL_H
#define DISPLACE_SIMPLEINCREMENTALMODEL_H

#include <strategies/modelstrategy.h>

namespace displace {
    namespace strategy {

        namespace model {

            class SimpleIncrementalModel : public ModelStrategy {
            public:
                SimpleIncrementalModel() : mFactor(1.3f) { }

                virtual bool updatePopulationModel(Simulation *simulation) override;


            private:
                float mFactor;
            };

        }

    }
}


#endif //DISPLACE_SIMPLEINCREMENTALMODEL_H
