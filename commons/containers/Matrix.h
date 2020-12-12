//
// Created by happycactus on 12/12/20.
//

#ifndef DISPLACE_MATRIX_H
#define DISPLACE_MATRIX_H

#include "containers/Vector.h"

template<typename Value>
class Matrix {
public:
    using Row = Vector<Value>;

    Row const &operator[](int row) const
    {
        return rows.at(row);
    }

    Row &operator[](int row)
    {
        return rows.at(row);
    }

    Row &at(int rowIndex)
    {
        while (rows.size() <= rowIndex) {
            rows.push_back(Row{});
        }
        return rows[rowIndex];
    }


    Value &at(int rowIndex, int colIndex)
    {
        return at(rowIndex).at(colIndex);
    }

private:
    std::vector<Row> rows;
};


#endif //DISPLACE_MATRIX_H
