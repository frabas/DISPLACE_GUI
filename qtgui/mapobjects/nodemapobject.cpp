#include "nodemapobject.h"

#include <displacemodel.h>
#include <inputfileparser.h>

#include <mapobjectscontroller.h>
#include <mapobjects/nodegraphics.h>
#include <mapobjects/nodedetailswidget.h>

#include <QMapControl/QMapControl.h>

#include <stdexcept>

NodeMapObject::NodeMapObject(MapObjectsController *controller, int indx, Role role, std::shared_ptr<NodeData> node)
    : mController(controller),
      mNode(node),
      mRole(role),
      mGeometry(),
      mWidget(0)
{
    switch (role) {

    case GraphNodeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeGraphics(mNode.get(), mController, indx));
        mGeometry->setFlags(qmapcontrol::Geometry::IsSelectable);
        break;

    case GraphNodeWithPopStatsRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Population, mNode.get(), mController, indx));
        break;

    case GraphNodeWithBiomass:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Biomass, mNode.get(), mController, indx));
        break;

    case GraphNodeWithPopImpact:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Impact, mNode.get(), mController, indx));
        break;

    case GraphNodeWithBenthosBiomass:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::BenthosBiomass, mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumFTimeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumFTimeGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumSweptAreaRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumSweptAreaGraphics(mNode.get(), mController, indx));
        break;

    default:
        Q_ASSERT(false);        /* Disallow creating "unknown" nodes */
        break;
    }

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

bool NodeMapObject::showProperties()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    updateProperties();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

void NodeMapObject::updateProperties()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1 <b>id</b>: %4<br/>"
                           "<b>Coords: </b>%2 %3<br/>"
                           "<b>Landscape: </b>%5<br/>"
                           "<b>Code Area: </b>%6<br/>")
            .arg(QString::fromStdString(mNode->get_name()))
            .arg(mNode->get_y())
            .arg(mNode->get_x())
            .arg(mNode->get_idx_node())
            .arg(mNode->get_marine_landscape())
            .arg(mNode->get_code_area());

    switch (mRole) {
    default:
    case GraphNodeRole:
        if (mNode->getModel()->isShortestPathFolderLinked()) {
            InputFileParser parser;

            if (parser.isShortestPathPresent(mNode->getModel()->linkedShortestPathFolder(), mNode->get_idx_node())) {
                connect (mWidget, SIGNAL(toolButtonClicked()), this, SLOT(toolButtonClicked()));
                mWidget->showTool(true);
            }
        }
        text += QString("<br/><b>Adjacencies</b><br/>");
        for (int i = 0; i < mNode->getAdiacencyCount(); ++i) {
            std::shared_ptr<NodeData::Edge> e = mNode->getAdiacencyByIdx(i);
            std::shared_ptr<NodeData> t = e->target.lock();
            if (t.get() != nullptr)
                text += QString("Node <b>%1</b> weight <b>%2</b><br/>")
                        .arg(t->get_idx_node())
                        .arg(e->weight);
        }
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

    case GraphNodeWithBenthosBiomass:
        text += updateStatText("Benthos Biomass");
        break;

    case GraphNodeWithCumFTimeRole:
        text += QString("<br/><b>Fishing Effort (hours):</b> %1<br/>")
                .arg(mNode->get_cumftime());
        break;

    case GraphNodeWithCumSweptAreaRole:
        text += QString("<br/><b>Swept area (km2):</b> %1<br/>")
                .arg(mNode->get_cumsweptarea());
        break;
    }

    mWidget->setText(text);
}

void NodeMapObject::onSelectionChanged()
{
    mGeometry->requestRedraw();
    emit nodeSelectionHasChanged(this);
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
        case GraphNodeWithBenthosBiomass:
            val = mNode->getBenthosBiomass(i);
            break;
        default:
            throw std::runtime_error("Unhandled case in updateStatText");
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

void NodeMapObject::toolButtonClicked()
{
    QMessageBox::warning(mWidget, tr("Unimplemented"), tr("This function is not implemented yet."));
}

void NodeMapObject::widgetClosed()
{
    mWidget = 0; // detach
}
