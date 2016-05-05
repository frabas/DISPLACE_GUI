#include "setnodevaluecommand.h"

#include <QGraphicsItem>
#include <graphnodeitem.h>

SetNodeValueCommand::SetNodeValueCommand(QList<QGraphicsItem *> items, double new_value)
    : mNewValue(new_value)
{
    foreach (QGraphicsItem *i, items) {
        GraphNodeItem *item = dynamic_cast<GraphNodeItem *>(i);

        // don't like this - TODO: fix it without using downcasting
        if (item) {
            std::shared_ptr<dtree::Node> node = item->getNode();
            if (node.get() != 0) {
                Data d;
                d.node = node;
                d.item = item;
                d.old_value = node->value();
                mData.push_back(d);
            }
        }
    }

}

SetNodeValueCommand::~SetNodeValueCommand()
{
}



bool SetNodeValueCommand::execute()
{
    foreach (Data d, mData) {
        d.node->setValue(mNewValue);
        d.item->update();
    }
    return true;
}

bool SetNodeValueCommand::undo()
{
    foreach (Data d, mData) {
        d.node->setValue(d.old_value);
        d.item->update();
    }
    return true;
}

bool SetNodeValueCommand::redo()
{
    foreach (Data d, mData) {
        d.node->setValue(mNewValue);
        d.item->update();
    }
    return true;
}
