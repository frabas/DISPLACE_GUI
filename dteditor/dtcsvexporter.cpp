#include "dtcsvexporter.h"

#include <QQueue>
#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>

DtCsvExporter::DtCsvExporter()
{
}

bool DtCsvExporter::exportTree(QTextStream &stream, dtree::DecisionTree *tree, DtGraphicsScene *scene)
{
    QQueue<boost::shared_ptr<dtree::Node> >queue;
    QQueue<int> queueid;

    boost::shared_ptr<dtree::Node> node = tree->root();
    if (!node)
        return false;

    queue.push_back(node);
    queueid.push_back(0);

    int id = 0, nid = 0;
    while (!queue.isEmpty()) {
        node = queue.front();
        queue.pop_front();
        id = queueid.front();
        queueid.pop_front();

        stream << id << "," << dtree::VariableNames::variableName(node->variable()) << ",";
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            ++nid;
            boost::shared_ptr<dtree::Node> chld = node->getChild(i);
            if (chld) {
                stream << nid;
                queue.push_back(chld);
                queueid.push_back(nid);
            }
            stream << ",";
        }

        stream << endl;
    }

    return true;
}
