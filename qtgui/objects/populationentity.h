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
#ifndef POPULATIONENTITY_H
#define POPULATIONENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class PopulationEntity : public ObjectTreeEntity
{
    int mPopulationIndex;

public:
    PopulationEntity(ObjectTreeModel *_model, int idx = -1);
    virtual ~PopulationEntity();

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Populations;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    QModelIndex makeIndex();
};

class PopulationEntityWithGroup : public ObjectTreeEntity {

    PopulationEntity *mParent;
    int mGroupIndex;

public:
    PopulationEntityWithGroup(ObjectTreeModel *_model, const PopulationEntity *parent, int idx = -1);
    virtual ~PopulationEntityWithGroup();

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Populations;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
};

}

#endif // POPULATIONENTITY_H
