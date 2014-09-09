#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT;
public:
    MyGraphicsView(QWidget* parent = NULL);
    //Set the current centerpoint in the
    void SetCenter(const QPointF& centerPoint);
    QPointF GetCenter() { return CurrentCenterPoint; }

protected:
    //Holds the current centerpoint for the view, used for panning and zooming
    QPointF CurrentCenterPoint;

    //From panning the view
    QPoint LastPanPoint;


    //Take over the interaction
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
};

#endif // MYGRAPHICSVIEW_H

