//
// Created by happycactus on 12/12/20.
//

#ifndef DISPLACE_MATRIX_H
#define DISPLACE_MATRIX_H

#include "containers/Vector.h"

template<typename Value>
class Matrix3 {
public:
    Matrix3(size_t i = 1, size_t j = 1, size_t k = 1)
            : di(i), dj(j), dk(k), m(di * dj * dk)
    {

    }

    Matrix3(Matrix3<Value> &&) = default;
    Matrix3(Matrix3<Value> &) = delete;

    ~Matrix3() = default;

    Matrix3<Value> &operator=(Matrix3<Value> &&) = default;
    Matrix3<Value> &operator=(Matrix3<Value> &) = delete;

    Value &at(size_t i, size_t j, size_t k)
    {
        return m[index(i, j, k)];
    }

    Value const &at(size_t i, size_t j, size_t k) const
    {
        return m[index(i, j, k)];
    }

private:
    size_t index(size_t i, size_t j, size_t k) const
    {
        if (i < di && j < dj && k < dk) {
            return k * dj * di + j * di + i;
        }
        std::ostringstream ss;
        ss << "Matrix indexes out of range: [" << i << "," << j << "," << k << "] limits "
           << di << "," << dj << "," << dk;
        throw std::out_of_range(ss.str());
    }

    size_t di, dj, dk;
    std::vector<Value> m;
};


#endif //DISPLACE_MATRIX_H
