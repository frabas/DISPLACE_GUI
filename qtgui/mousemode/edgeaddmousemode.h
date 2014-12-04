#ifndef EDGEADDMOUSEMODE_H
#define EDGEADDMOUSEMODE_H

#include "mousemode.h"
#include <QObject>

class DisplaceModel;

class EdgeAddMouseMode : public MouseMode
{
    Q_OBJECT
public:
    explicit EdgeAddMouseMode(DisplaceModel *model);

    bool releaseEvent(const QPointF &point);

    bool beginMode();
    bool endMode(bool success);

    QString getModeDescription() const;
signals:
    void edgeAdded (int from_idx, int to_idx);

private:
    DisplaceModel *mModel;
    int mStatus;        ///< 0: started, 1: first point selected
    int mNodeFrom, mNodeTo;
};

#endif // EDGEADDMOUSEMODE_H
