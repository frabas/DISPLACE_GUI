//
// Created by happycactus on 12/12/20.
//

#ifndef DISPLACE_VECTOR_H
#define DISPLACE_VECTOR_H

#include <vector>

template<typename Value>
class Vector {
    std::vector<Value> values;
public:

    Value const &operator[](int index) const
    { return values.at(index); }

    Value &operator[](int index)
    { return values.at(index); }

    Value &at(int index)
    {
        while (values.size() <= index) {
            values.push_back(Value{});
        }
        return values.at(index);
    }
};


#endif //DISPLACE_VECTOR_H
