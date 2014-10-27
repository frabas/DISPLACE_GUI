#ifndef EDGELAYER_H
#define EDGELAYER_H

#include <QObject>

#include <QMapControl/LayerGeometry.h>
#include <mapobjectscontroller.h>

class EdgeMapObject;

class EdgeLayer : public QObject
{
    Q_OBJECT
public:
    explicit EdgeLayer(MapObjectsController *controller, const QString &name, QObject *parent = 0);

    void setVisible(bool vi) { mLayer->setVisible(vi); }
    void addEdge (EdgeMapObject *edge);

    std::shared_ptr<qmapcontrol::LayerGeometry> layer() const { return mLayer; }

    const QList<EdgeMapObject *>selection() const { return mSelection; }

private:
    MapObjectsController *mController;
    std::shared_ptr<qmapcontrol::LayerGeometry> mLayer;

    QList<EdgeMapObject *> mSelection;
};

#endif // EDGELAYER_H
