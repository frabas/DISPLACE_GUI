#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <memory>

#include <QList>
#include <QString>
#include <QPair>

#include <graphbuilder.h>

class HarbourData;

class InputFileParser
{
public:
    InputFileParser();

    bool parseHarbourFile (const QString &path, QList<std::shared_ptr<HarbourData> > &list, QString *error);
    bool parseShortestPath (const QString &path, int nodeid, QList<QPair<int, int> > &graph);
    bool isShortestPathPresent (const QString &path, int nodeid);

    bool parseGraph(const QString &graphpath, const QString &coordsPath, QList<GraphBuilder::Node> &nodes, QString *error);
};

#endif // INPUTFILEPARSER_H
