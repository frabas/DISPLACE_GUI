#include "drawpenaltypolygon.h"

#include <mainwindow.h>

#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QLayout>

#include <QMapControl/GeometryPolygon.h>

DrawPenaltyPolygon::DrawPenaltyPolygon(MainWindow *win, EditorLayerInterface *ifc)
    : mMainWindow(win),
      mLayerInterface(ifc),
      mPressed(false)
{
    mActionDialog = new QDialog(mMainWindow);

    QHBoxLayout *layout = new QHBoxLayout();
    mActionDialog->setLayout(layout);

    QPushButton *b = new QPushButton(QObject::tr("Abort"));
    QObject::connect (b, SIGNAL(clicked()), mMainWindow, SLOT(abortMouseMode()));

    layout->addWidget(b);
    b = new QPushButton(QObject::tr("Completed"));
    QObject::connect (b, SIGNAL(clicked()), mMainWindow, SLOT(completeMouseMode()));
    layout->addWidget(b);
}

DrawPenaltyPolygon::~DrawPenaltyPolygon()
{
    delete mActionDialog;
}

bool DrawPenaltyPolygon::pressEvent(const QPointF &point)
{
    Q_UNUSED(point);

    mPressed = true;
    return true;
}

bool DrawPenaltyPolygon::releaseEvent(const QPointF &point)
{
    if (mPressed) {
        qDebug() << "NewPoint: " << point;

        mPoints << point;

        std::vector<qmapcontrol::PointWorldCoord> polypoints;
        foreach (const QPointF &pt , mPoints) {
            polypoints.push_back(qmapcontrol::PointWorldCoord(pt.x(), pt.y()));
        }

        std::shared_ptr<qmapcontrol::GeometryPolygon> poly(new qmapcontrol::GeometryPolygon(polypoints));

        poly->setPen(QPen(QBrush(Qt::red), 2));
        poly->setBrush(QBrush(QColor::fromRgba(qRgba(255,0,0,128))));
        mLayerInterface->clearEditorLayer();
        mLayerInterface->addEditorLayerGeometry(poly);
    }
    mPressed = false;
    return true;
}

bool DrawPenaltyPolygon::moveEvent(const QPointF &point)
{
    Q_UNUSED(point);

    if (mPressed)
        mPressed = false;
    return true;
}

bool DrawPenaltyPolygon::beginMode()
{
    mActionDialog->show();
    return true;
}

bool DrawPenaltyPolygon::endMode(bool success)
{
    if (success) {
        qDebug() << mPoints.size() << "Points polygon";
    }

    mActionDialog->hide();
    mPoints.clear();
    return true;
}
