#include "mapview.h"
#include <QAbstractScrollArea>

MapView::MapView(QWidget *parent)
: QGraphicsView(parent), lastMouseEvent(QEvent::None,QPoint(),
Qt::NoButton,Qt::NoButton,Qt::NoModifier)

{
setAttribute (Qt::WA_SetCursor);
setCursor(Qt::CrossCursor);
coordinateDisplayLabel = new QLabel(0);
setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff) ;
setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

scrolling = false;
}

void MapView::wheelEvent(QWheelEvent *event)
{
scaleView(pow((double)2, event->delta() / 240.0));
}

void MapView::scaleView(qreal scaleFactor)
{
qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
if (factor < 0.07 || factor > 100)
return;
scale(scaleFactor, scaleFactor);
}

void MapView::storeMouseEvent(QMouseEvent *event)
{
lastMouseEvent = QMouseEvent(event->type(), event->pos(), event->globalPos(),
event->button(),event->buttons(),event->modifiers());
}

void MapView::mousePressEvent(QMouseEvent *event)
{
if(event->button() == Qt::MidButton)
{
setCursor(Qt::ClosedHandCursor);
scrolling = true;
storeMouseEvent(event);
event->accept();
return;
}
}

void MapView::mouseMoveEvent(QMouseEvent *event)
{
if(scrolling)
{
QPoint delta = event->globalPos() - lastMouseEvent.globalPos();
QScrollBar *hBar = horizontalScrollBar();
QScrollBar *vBar = verticalScrollBar();
hBar->setValue(hBar->value() - delta.x());
vBar->setValue(vBar->value() - delta.y());
storeMouseEvent(event);
event->accept();
return;
}

sceneCoordinates = mapFromScene(event->pos());
cursorXString.setNum(sceneCoordinates.x());
cursorYString.setNum(sceneCoordinates.y());
coordinateDisplayLabel->setText("x = " + cursorXString + " " + "y = " + cursorYString);
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
if((event->button() == Qt::MidButton) && (scrolling == true))
{
setCursor(Qt::CrossCursor);
scrolling = false;
storeMouseEvent(event);
event->accept();
return;
}
event->ignore();
}

