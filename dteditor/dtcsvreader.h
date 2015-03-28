#ifndef DTCSVREADER_H
#define DTCSVREADER_H

#include <QTextStream>
#include <boost/shared_ptr.hpp>

#include <stdexcept>

namespace dtree {
class DecisionTree;
}

class DtGraphicsScene;

class DtCsvReader
{
public:
    DtCsvReader();
    bool readTree (QTextStream &stream, boost::shared_ptr<dtree::DecisionTree> *tree, DtGraphicsScene *scene)
    throw (std::invalid_argument);
};

#endif // DTCSVREADER_H
