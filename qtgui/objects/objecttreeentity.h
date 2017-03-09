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
#ifndef OBJECTTREEENTITY_H
#define OBJECTTREEENTITY_H

#include <QModelIndex>
#include <objecttreemodel.h>

class ObjectTreeModel;
class QMenu;

namespace objecttree {

class ObjectTreeEntity
{
protected:
    ObjectTreeModel *model;

public:
    ObjectTreeEntity(ObjectTreeModel *_model);
    virtual ~ObjectTreeEntity();

    virtual ObjectTreeModel::Category getCategory() const = 0;

    virtual QModelIndex parent (const QModelIndex &parent) const = 0;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const = 0;
    virtual int rowCount() const = 0;
    virtual int columnCount() const = 0;
    virtual QVariant data(const QModelIndex &index, int role) const = 0;

    virtual Qt::ItemFlags flags(Qt::ItemFlags defFlags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    virtual QMenu *contextMenu() const;

    virtual void modelChanged(int) {}
};

}

#endif // OBJECTTREEENTITY_H
