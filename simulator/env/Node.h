//
// Created by Federico Fuga on 04/03/16.
//

#ifndef DISPLACE_NODE_H
#define DISPLACE_NODE_H

#include <memory>

namespace displace {
    namespace env {
        namespace node {
            struct PopulationsAspect;
        }

        class Node {
        private:
            long mId = -1;
            double mX = 0, mY = 0;
        public:
            explicit Node(long id);

            ~Node() noexcept;

            long getId() const { return mId; }
            void setX(double x) { mX = x; }
            void setY(double y) { mY = y; }
            double x() const { return mX; }
            double y() const { return mY; }

            node::PopulationsAspect &populationsAspect() { return *mPopulationsAspect; }
            const node::PopulationsAspect &populationsAspect() const { return *mPopulationsAspect; }

        private:
            std::unique_ptr<node::PopulationsAspect> mPopulationsAspect;
        };

    }
}



#endif //DISPLACE_NODE_H
