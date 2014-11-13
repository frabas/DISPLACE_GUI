#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <memory>

#include <QList>
#include <QString>
#include <QPair>

class HarbourData;

class InputFileParser
{
public:
    InputFileParser();

    bool parseHarbourFile (const QString &path, QList<std::shared_ptr<HarbourData> > &list, QString *error);
    bool parseShortestPath (const QString &path, int nodeid, QList<QPair<int, int> > &graph);
    bool isShortestPathPresent (const QString &path, int nodeid);
};

#endif // INPUTFILEPARSER_H
