// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "shapefilelayerentity.h"
#include <objecttreemodel.h>

#include <displacemodel.h>
#include <objecttreemodel.h>
#include <mapobjectscontroller.h>

#include <QColorDialog>
#include <QDebug>

using namespace objecttree;

ShapefileLayerEntity::ShapefileLayerEntity(ObjectTreeModel *_model, int LayerEntity_idx)
    : LayerEntity(ObjectTreeModel::ShapefileLayers, _model, LayerEntity_idx),
      mContextMenu(0)
{
}

ShapefileLayerEntity::~ShapefileLayerEntity()
{

}

QModelIndex ShapefileLayerEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new ShapefileLayerEntity(model, row);
    return model->createEntity(row, column, entity);
}

QMenu *ShapefileLayerEntity::contextMenu() const
{
    if (layerEntityIndex() == -1 || layerEntityType() != ObjectTreeModel::ShapefileLayers)
        return 0;

    if (mContextMenu == 0) {
        const_cast<ShapefileLayerEntity*>(this)->initMenu();
    }

    return mContextMenu;
}

void ShapefileLayerEntity::onActionShapeColor()
{
    std::shared_ptr<qmapcontrol::ESRIShapefile> file = model->getMapControl()->getShapefile(model->getModelIdx(), layerEntityIndex());
    QBrush brh = file->getBrushPolygon();

    QColor color = QColorDialog::getColor(brh.color(), 0, tr("Select shapefile color"), QColorDialog::ShowAlphaChannel);

    if (color.isValid()) {
        brh.setColor(color);
        file->setBrushPolygon(brh);
        model->getMapControl()->redraw();
    }
}

void ShapefileLayerEntity::initMenu()
{
    mContextMenu = new QMenu();
    connect (mContextMenu->addAction(QObject::tr("Shape Color...")), SIGNAL(triggered()), this, SLOT(onActionShapeColor()));
}
