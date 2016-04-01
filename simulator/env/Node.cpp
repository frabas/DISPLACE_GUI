//
// Created by Federico Fuga on 04/03/16.
//

#include "Node.h"
#include <env/aspects/populationsaspect.h>
#include <utils/make_unique.h>

using namespace displace::env;

Node::Node(long id)
        : mId(id)
{
    mPopulationsAspect = utils::make_unique<node::PopulationsAspect>();
}

Node::~Node() noexcept = default;




