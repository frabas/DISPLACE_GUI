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
        mContextMenu = new QMenu();
        connect (mContextMenu->addAction(QObject::tr("Shape Color...")), SIGNAL(triggered()), this, SLOT(onActionShapeColor()));
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
