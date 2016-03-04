//
// Created by Federico Fuga on 04/03/16.
//

#ifndef DISPLACE_EDGE_H
#define DISPLACE_EDGE_H

namespace displace {
    namespace env {
        class Edge {
        private:
            long mIdx = -1;
            double mWeight = 0.0;

        public:
            explicit Edge(int idx = -1)
            : mIdx(idx) {
            }

            ~Edge() noexcept;

            void setWeight(double w) { mWeight = w; }
            double weight() const { return mWeight; }
        };
    }
}

#endif //DISPLACE_EDGE_H
