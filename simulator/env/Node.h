//
// Created by Federico Fuga on 04/03/16.
//

#ifndef DISPLACE_NODE_H
#define DISPLACE_NODE_H

namespace displace {
    namespace env {

        class Node {
        private:
            long mId = -1;
            double mX = 0, mY = 0;
        public:
            explicit Node(long id)
            : mId(id) {
            }

            ~Node() noexcept = default;

            long getId() const { return mId; }
            void setX(double x) { mX = x; }
            void setY(double y) { mY = y; }
            double x() const { return mX; }
            double y() const { return mY; }
        };

    }
}



#endif //DISPLACE_NODE_H
