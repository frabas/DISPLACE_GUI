#ifndef MAPVIEW_H
#define MAPVIEW_H


#include <QStatusBar>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QScrollBar>
#include <QLabel>
#include <math.h>

class MapView : public QGraphicsView
{

public:
MapView(QWidget *parent = 0);
QPoint sceneCoordinates;
QLabel *coordinateDisplayLabel;

protected:
void scaleView(qreal scaleFactor);
void wheelEvent(QWheelEvent *event);
void mousePressEvent(QMouseEvent *event);
void mouseMoveEvent(QMouseEvent *event);
void mouseReleaseEvent(QMouseEvent *event);

private:
QString cursorXString;
QString cursorYString;
void storeMouseEvent(QMouseEvent *event);
bool scrolling;
QMouseEvent lastMouseEvent;

};


#endif // MAPVIEW_H


