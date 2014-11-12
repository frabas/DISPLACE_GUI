#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <QList>
#include <memory>
#include <QString>

class HarbourData;

class InputFileParser
{
public:
    InputFileParser();

    bool parseHarbourFile (const QString &path, QList<std::shared_ptr<HarbourData> > &list, QString *error);
};

#endif // INPUTFILEPARSER_H
