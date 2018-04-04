#ifndef DBTYPES_H
#define DBTYPES_H

#include <vector>

// TODO Move this into the types namespace
struct TimelineData {
    std::vector<double> t;
    std::vector<double> v;
};

#endif // DBTYPES_H
