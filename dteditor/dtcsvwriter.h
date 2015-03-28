#ifndef DTCSVEXPORTER_H
#define DTCSVEXPORTER_H

#include <QTextStream>

namespace dtree {
class DecisionTree;
}

class DtGraphicsScene;

class DtCsvWriter
{
public:
    DtCsvWriter();

    bool exportTree (QTextStream &stream, dtree::DecisionTree *tree, DtGraphicsScene *scene);
};

#endif // DTCSVEXPORTER_H
