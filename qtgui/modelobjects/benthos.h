#ifndef BENTHOS_H
#define BENTHOS_H

#include <QMap>
#include <QString>

class NodeData;

class Benthos
{
    int mBenthosId;
    QList<NodeData *> mNodes;

public:
    explicit Benthos(int id);

    int getId() const { return mBenthosId; }
    const QList<NodeData *> getNodes() const { return mNodes; }

    void setNodes(const QList<NodeData *> &nodes);
    void appendNode (NodeData *node);
};

#endif // BENTHOS_H
