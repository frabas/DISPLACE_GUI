#include "nodemapobject.h"

#include <displacemodel.h>
#include <mapobjectscontroller.h>
#include <mapobjects/nodegraphics.h>
#include <QMapControl/QMapControl.h>

#include <mapobjects/nodedetailswidget.h>

NodeMapObject::NodeMapObject(MapObjectsController *controller, int indx, Role role, NodeData *node)
    : mController(controller),
      mNode(node),
      mGeometry()
{
    switch (role) {

    case GraphNodeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeGraphics(mNode, mController, indx));
        break;

    case GraphNodeWithPopStatsRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Population, mNode, mController, indx));
        break;

    case GraphNodeWithBiomass:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Biomass, mNode, mController, indx));
        break;

    case GraphNodeWithPopImpact:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Impact, mNode, mController, indx));
        break;

    case GraphNodeWithCumFTimeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumFTimeGraphics(mNode, mController, indx));
        break;

    default:
        Q_ASSERT(false);        /* Disallow creating "unknown" nodes */
        break;
    }

    mGeometry->setAncillaryData(this);
}

bool NodeMapObject::clicked()
{
    NodeDetailsWidget *w = new NodeDetailsWidget(mController->mapWidget());
    w->setText(QString("<b>Name</b>: %1<br/>"
                        "<b>Coords: </b>%2 %3<br/>"
                        )
                                .arg(QString::fromStdString(mNode->get_name()))
                                .arg(mNode->get_y())
                                .arg(mNode->get_x()));

    mController->showDetailsWidget(mGeometry->coord(),
                                w);

    return true;
}
