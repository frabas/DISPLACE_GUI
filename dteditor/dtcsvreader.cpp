#include "dtcsvreader.h"
#include <graphnodeitem.h>

#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>

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
        int parent;
        QPointF position;
        QVector<int> children;
    };

    QVector<int> parents;
    QVector<Data> data;
    bool ok;

    while (!stream.atEnd()) {
        line = stream.readLine();

        if (line.startsWith('#'))
            continue;

        QStringList fields = line.split(',', QString::KeepEmptyParts);
        int idx = fields[0].toInt(&ok);
        if (!ok) throw std::invalid_argument("invalid field");

        while(data.size() <= idx)
            data.push_back(Data());
        data[idx].node = tree->createNode();

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

            while (parents.size() <= cidx)
                parents.push_back(-1);
            parents[cidx] = idx;
        }
    }

    // TODO: build the dtree based on the content of data

    return true;
}
