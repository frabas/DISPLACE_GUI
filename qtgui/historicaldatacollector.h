// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef HISTORICALDATACOLLECTOR_H
#define HISTORICALDATACOLLECTOR_H

#include <QMap>
#include <QLinkedList>
#include <memory>

template <typename T>
class HistoricalDataCollector {
public:
    typedef T Value;
    typedef QMap<int, Value> Container;

    HistoricalDataCollector() {
    }

    void insertValue (int tstep, const Value &data) {
        mData.insert(tstep, data);
    }

    const Value &getValue(int tstep) const {
        typename Container::const_iterator it = mData.lowerBound(tstep+1);
        --it;
        return it.value();
    }

    int getUniqueValuesCount() const {
        return mData.size();
    }

    typename Container::const_iterator getFirst() const {
        return mData.begin();
    }
    bool atEnd(typename Container::const_iterator it) const {
        return it == mData.end();
    }

    void clear() { mData.clear(); }

protected:
    Container mData;
};

#endif // HISTORICALDATACOLLECTOR_H
