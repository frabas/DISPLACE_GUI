//
// Created by happycactus on 1/10/20.
//

#include "SimModel.h"

struct SimModel::Impl {

};

SimModel::SimModel()
        : p(spimpl::make_unique_impl<Impl>())
{

}
