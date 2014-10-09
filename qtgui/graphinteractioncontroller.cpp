#include "graphinteractioncontroller.h"

#include <QDebug>

GraphInteractionController::GraphInteractionController(QCustomPlot *plot, QObject *parent) :
    QObject(parent),
    mPlot(plot)
{
    mPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                           QCP::iSelectLegend | QCP::iSelectPlottables);

    //connect(mPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    connect(mPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(mPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // setup policy and connect slot for context menu popup:
    mPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

void GraphInteractionController::selectionChanged()
{

}

void GraphInteractionController::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (mPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        mPlot->axisRect()->setRangeDrag(mPlot->xAxis->orientation());
    else if (mPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        mPlot->axisRect()->setRangeDrag(mPlot->yAxis->orientation());
    else
        mPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);

}

void GraphInteractionController::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (mPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        mPlot->axisRect()->setRangeZoom(mPlot->xAxis->orientation());
    else if (mPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        mPlot->axisRect()->setRangeZoom(mPlot->yAxis->orientation());
    else
        mPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void GraphInteractionController::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(mPlot);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (mPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction(tr("Move to top left"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction(tr("Move to top center"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction(tr("Move to top right"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction(tr("Move to bottom right"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction(tr("Move to bottom left"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    }

    menu->popup(mPlot->mapToGlobal(pos));
}

void GraphInteractionController::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            mPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            mPlot->replot();
        }
    }
}
