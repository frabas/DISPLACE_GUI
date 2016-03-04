//
// Created by Federico Fuga on 03/03/16.
//

#include <Environment.h>
#include <utils/make_unique.h>
#include <env/Playground.h>

using namespace displace;

struct Environment::Impl {
};

Environment::Environment()
: mImpl( utils::make_unique<Impl>() ),
  mPlayground(utils::make_unique<env::Playground>())
{

}

Environment::~Environment() noexcept = default;

