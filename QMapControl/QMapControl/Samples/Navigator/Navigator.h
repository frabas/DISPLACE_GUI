//
// Created by fuga on 18/07/2020.
//

#ifndef QMAPCONTROL_DEMO_NAVIGATOR_H
#define QMAPCONTROL_DEMO_NAVIGATOR_H

#include "QMapControl/Projection.h"

#include <QtWidgets/QMainWindow>

#include <memory>

class Navigator : public QMainWindow {
Q_OBJECT

    struct Impl;
    Impl *p;
public:
    Navigator();

    virtual ~Navigator();

private:

    void buildMenu();

    void buildOnMapControls();

public slots:

    void onActionRecordPoint();

    void onActionSavePath();

    void onActionLoadPath();

    void onActionLoadShapefile();

    void onActionLoadTiff();

    void onActionPlayPath();

    void onCourseChanged(qreal newcourse);

    void animate();

    void updatePath();

    void mapFocusPointChanged(qmapcontrol::PointWorldCoord);

    void mapMouseMove(QMouseEvent *, qmapcontrol::PointWorldCoord, qmapcontrol::PointWorldCoord);

    void mapRotationChanged(qreal courseDegrees);

    void updateLayersMenu();
};

#endif // QMAPCONTROL_DEMO_NAVIGATOR_H
