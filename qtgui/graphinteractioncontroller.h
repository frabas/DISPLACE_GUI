#ifndef GRAPHINTERACTIONCONTROLLER_H
#define GRAPHINTERACTIONCONTROLLER_H

#include <QObject>
#include <qcustomplot.h>

class GraphInteractionController : public QObject
{
    Q_OBJECT

    QCustomPlot *mPlot;
public:
    explicit GraphInteractionController(QCustomPlot *plot, QObject *parent = 0);

signals:

public slots:
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
};

#endif // GRAPHINTERACTIONCONTROLLER_H
