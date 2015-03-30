#include "dtcsvreader.h"
#include <graphnodeitem.h>

#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>

#include <QQueue>
#include <QDebug>

const int DtCsvReader::VERSION = 1;

DtCsvReader::DtCsvReader()
{
}

bool DtCsvReader::readTree(QTextStream &stream, boost::shared_ptr<dtree::DecisionTree> *res_tree, DtGraphicsScene *scene)
throw (std::invalid_argument)
{
    boost::shared_ptr<dtree::DecisionTree> tree (new dtree::DecisionTree());

    QString line;

    struct Data {
        boost::shared_ptr<dtree::Node> node;
        GraphNodeItem *item;
        int parent;
        QPointF position;
        QVector<int> children;
    };

    QVector<Data> data;
    bool ok;
    int version = 0;

    while (!stream.atEnd()) {
        line = stream.readLine();

        if (line.startsWith('#')) {
            QStringList fields = line.split(' ');
            if (fields[0] == "#DTreeVersion:") {
                version = fields[1].toInt(&ok);
                if (!ok)
                    throw std::invalid_argument("Invalid version token");
                if (version > VERSION)
                    throw std::invalid_argument("Unsupported version");
            }
            continue;
        }

        QStringList fields = line.split(',', QString::KeepEmptyParts);
        int idx = fields[0].toInt(&ok);
        if (!ok) throw std::invalid_argument("invalid field");

        while(data.size() <= idx)
            data.push_back(Data());
        data[idx].node = tree->createNode();
        data[idx].item = new GraphNodeItem(data[idx].node, scene);

        dtree::Variable var = dtree::VariableNames::variableCode(fields[1].toStdString());
        data[idx].node->setVariable(var);

        double x = fields[2].toDouble(&ok);
        if (!ok) throw std::invalid_argument("invalid field");
        double y = fields[3].toDouble(&ok);
        if (!ok) throw std::invalid_argument("invalid field");

        data[idx].position = QPointF(x,y);

        int nc = fields[4].toInt(&ok);
        if (!ok) throw std::invalid_argument("invalid field");
        while (data[idx].children.size() < nc)
            data[idx].children.push_back(-1);

        for (int i = 0; i < nc; ++i) {
            int cidx = fields[5+i].toInt(&ok);
            if (!ok) continue;

            data[idx].children[i] = cidx;

            while (data.size() <= cidx)
                data.push_back(Data());
            data[cidx].parent = idx;
        }
    }

    for (int di = 0; di < data.size(); ++di) {
        Data d = data[di];

        Q_ASSERT(d.node.get());
        Q_ASSERT(d.item);
        if (tree->isEmpty()) {
            tree->setRoot(d.node);
            scene->addItemAsRoot(d.item);
        } else {
            d.node->setParent(data[d.parent].node);
            scene->addItem(d.item);
        }

        for(int i = 0; i < d.children.size(); ++i) {
            int chl = d.children[i];
            if (chl != -1) { // children is valid
                tree->connect(data[chl].node, d.node, i);
                d.item->connectAsParent(data[chl].item, i);
            }
        }
        d.item->setPos(d.position);
    }

    *res_tree = tree;

    QQueue<GraphNodeItem *> q;
    q.push_back(scene->root());
    while (!q.empty()) {
        GraphNodeItem *item = q.front();
        q.pop_front();
        if (item) {
            for (int i = 0; i < item->getChildrenCount(); ++i) {
                q.push_back(item->getChild(i));
            }
            item->update();
        }
    }

    return true;
}
