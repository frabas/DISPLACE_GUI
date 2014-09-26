#include "benthos.h"

Benthos::Benthos(int id)
    : mBenthosId(id),
      mNodes()
{
}

void Benthos::setNodes(const QList<Node *> &nodes)
{
    mNodes = nodes;
}

void Benthos::appendNode(Node *node)
{
    mNodes.append(node);
}
