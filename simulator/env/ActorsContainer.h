//
// Created by <fuga@studiofuga.com> on 08/03/16.
//

#ifndef DISPLACE_ACTORSCONTAINER_H
#define DISPLACE_ACTORSCONTAINER_H

#include <vector>
#include <memory>

namespace displace {
namespace env {

template <typename T>
class ActorsContainer {
private:
    std::vector<std::shared_ptr<T>> mContainer;
public:
    ActorsContainer() {}
    virtual ~ActorsContainer() noexcept = default;

    int addActor(std::shared_ptr<T> actor) {
        int r = mContainer.size();
        mContainer.emplace_back(actor);
        return r;
    }

    int actorsCount() const {
        return mContainer.size();
    }

    T& actorById(int id) {
        return *mContainer[id].get();
    }
    const T& actorById(int id) const {
        return *mContainer[id].get();
    }
};

}
}

#endif //DISPLACE_ACTORSCONTAINER_H
