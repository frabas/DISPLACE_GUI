#include "benthos.h"

Benthos::Benthos(int id)
    : mBenthosId(id),
      mNodes()
{
}

void Benthos::setNodes(const QList<NodeData *> &nodes)
{
    mNodes = nodes;
}

void Benthos::appendNode(NodeData *node)
{
    mNodes.append(node);
}
