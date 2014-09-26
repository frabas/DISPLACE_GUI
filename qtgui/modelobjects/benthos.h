#ifndef BENTHOS_H
#define BENTHOS_H

#include <QMap>
#include <QString>

class Node;

class Benthos
{
    int mBenthosId;
    QList<Node *> mNodes;

public:
    explicit Benthos(int id);

    int getId() const { return mBenthosId; }
    const QList<Node *> getNodes() const { return mNodes; }

    void setNodes(const QList<Node *> &nodes);
    void appendNode (Node *node);
};

#endif // BENTHOS_H
