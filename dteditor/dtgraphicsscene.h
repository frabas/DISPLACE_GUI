#ifndef DTGRAPHICSSCENE_H
#define DTGRAPHICSSCENE_H

#include <QGraphicsScene>

#include <boost/shared_ptr.hpp>
#include <set>

namespace dtree {
class GraphNodeExtra;
class Node;
}


class DtGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DtGraphicsScene(QObject *parent = 0);

    void addUnconnectedNode(boost::shared_ptr<dtree::Node> node, QPointF pos);

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void nodeAddCompleted(QPointF);
    void mouseModeEnded();

public slots:
    void startAddNode();
    void endMode();

private:
    enum Mode {
        None, AddNode
    };

    Mode mMode = None;

    std::set<boost::shared_ptr<dtree::Node> > mUnconnectedNodes;
};

#endif // DTGRAPHICSSCENE_H
