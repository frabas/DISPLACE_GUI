// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "drawpenaltypolygon.h"

#include <mainwindow.h>

#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QLayout>
#include <QSettings>

#include <QMapControl/GeometryPolygon.h>
#include <QMapControl/GeometryPointCircle.h>

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

    QSettings set;
    if (set.contains("draw_penalties_win"))
        mActionDialog->move(set.value("draw_penalties_win").toPoint());
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
        mPoints << point;

        std::vector<qmapcontrol::PointWorldCoord> polypoints;
        foreach (const QPointF &pt , mPoints) {
            polypoints.push_back(qmapcontrol::PointWorldCoord(pt.x(), pt.y()));
        }

        mLayerInterface->clearEditorLayer();

        std::shared_ptr<qmapcontrol::GeometryPointCircle> circle(new qmapcontrol::GeometryPointCircle(qmapcontrol::PointWorldCoord(point.x(), point.y())));
        circle->setPen(QPen(Qt::blue));
        mLayerInterface->addEditorLayerGeometry(circle);

        std::shared_ptr<qmapcontrol::GeometryPolygon> poly(new qmapcontrol::GeometryPolygon(polypoints));

        poly->setPen(QPen(QBrush(Qt::red), 2));
        poly->setBrush(QBrush(QColor::fromRgba(qRgba(255,0,0,128))));
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

        mMainWindow->addPenaltyPolygon(mPoints);
    }

    QSettings set;
    set.setValue("draw_penalties_win", mActionDialog->pos());
    mActionDialog->hide();
    mPoints.clear();
    mLayerInterface->clearEditorLayer();
    return true;
}

QString DrawPenaltyPolygon::getModeDescription() const
{
    return QString(tr("Drawing Penalty Polygon Mode"));
}
