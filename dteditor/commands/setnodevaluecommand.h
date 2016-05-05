#ifndef SETNODEVALUECOMMAND_H
#define SETNODEVALUECOMMAND_H

#include <commands/command.h>
#include <memory>
#include <dtree/dtnode.h>
#include <QList>

class QGraphicsItem;
class GraphNodeItem;

class SetNodeValueCommand : public Command
{
public:
    SetNodeValueCommand(QList<QGraphicsItem *> items, double new_value);
    virtual ~SetNodeValueCommand();

    virtual bool execute();
    virtual bool undo();
    virtual bool redo();

private:
    struct Data {
        std::shared_ptr<dtree::Node> node;
        GraphNodeItem *item;
        double old_value;
    };

    QList<Data> mData;
    double mNewValue;
};

#endif // SETNODEVALUECOMMAND_H
