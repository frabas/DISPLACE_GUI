#include "nodeentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <QMapControl/QMapControl.h>
#include <mapobjectscontroller.h>

#include <QInputDialog>

namespace objecttree {

NodeEntity::NodeEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mNodeId(id),
      mContextMenu(0)
{
}

QModelIndex NodeEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Nodes);
}

QModelIndex NodeEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new NodeEntity(model, row);
    return model->createEntity(row, column, entity);
}

int NodeEntity::rowCount() const
{
    if (mNodeId == -1 && model->getModel() != 0)
        return model->getModel()->getNodesCount();

    return 0;
}

int NodeEntity::columnCount() const
{
    return 1;
}

QVariant NodeEntity::data(const QModelIndex &index, int role) const
{
    if (mNodeId != -1 && model->getModel() != 0 && role == Qt::DisplayRole && index.column() == 0) {
        return model->getModel()->getNodeId(mNodeId);
    }

    return QVariant();
}

Qt::ItemFlags NodeEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);

    return defflags;
}

bool NodeEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;   // item is not editable
}

QMenu *NodeEntity::contextMenu() const
{
    if (mContextMenu == 0) {
        mContextMenu = new QMenu();
        connect (mContextMenu->addAction(QObject::tr("Find node by Id...")), SIGNAL(triggered()), this, SLOT(onActionSearchById()));
    }

    return mContextMenu;
}

void NodeEntity::onActionSearchById()
{
    bool ok;
    int id = QInputDialog::getInt(0,
                                  tr("Search node by ID"),
                                  tr("Please enter the node ID"),
                                  mNodeId, 0, model->getModel()->getNodesCount()-1,
                                  1, &ok);


    if (ok) {
        std::shared_ptr<NodeData> n (model->getModel()->getNodesList()[id]);

        model->getMapControl()->mapWidget()->setMapFocusPointAnimated(
                    qmapcontrol::PointWorldCoord(n->get_x(), n->get_y()),
                    5, std::chrono::milliseconds(100));
    }
}


}
