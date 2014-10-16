#include "nodemapobject.h"

#include <displacemodel.h>
#include <mapobjectscontroller.h>
#include <mapobjects/nodegraphics.h>
#include <QMapControl/QMapControl.h>

#include <mapobjects/nodedetailswidget.h>

NodeMapObject::NodeMapObject(MapObjectsController *controller, int indx, Role role, NodeData *node)
    : mController(controller),
      mNode(node),
      mRole(role),
      mGeometry(),
      mWidget(0)
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

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

bool NodeMapObject::clicked()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    update();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

void NodeMapObject::update()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1<br/>"
                           "<b>Coords: </b>%2 %3<br/>")
            .arg(QString::fromStdString(mNode->get_name()))
            .arg(mNode->get_y())
            .arg(mNode->get_x());

    switch (mRole) {
    default:
    case GraphNodeRole:
        break;

    case GraphNodeWithPopStatsRole:
        text += updateStatText("Population");
        break;
    case GraphNodeWithBiomass:
        text += updateStatText("Biomass");
        break;
    case GraphNodeWithPopImpact:
        text += updateStatText("Impact");
        break;

    case GraphNodeWithCumFTimeRole:
        text += QString("<br/><b>Fishing Effort:</b> %1<br/>")
                .arg(mNode->get_cumftime());
        break;
    }

    mWidget->setText(text);
}

QString NodeMapObject::updateStatText(QString prefix)
{
    QString text = "<br/>";

    QList<int> ilist = mNode->getModel()->getInterestingPops();
    double tot = 0.0;

    foreach(int i, ilist) {
        double val;

        switch (mRole) {
        case GraphNodeWithPopStatsRole:
            val = mNode->getPop(i);
            break;
        case GraphNodeWithBiomass:
            val = mNode->getPopW(i);
            break;
        case GraphNodeWithPopImpact:
            val = mNode->getImpact(i);
            break;
        }

        text += QString("<b>%1 %2:</b> %3<br/>")
                .arg(prefix)
                .arg(i)
                .arg(val);
        tot += val;
    }
    text += QString("<b>Total:</b> %1<br/>")
            .arg(tot);

    return text;
}

void NodeMapObject::widgetClosed()
{
    mWidget = 0; // detach
}
