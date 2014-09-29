#ifndef OUTPUTFILEPARSER_H
#define OUTPUTFILEPARSER_H

#include <QString>
#include <QFile>

class MainWindow;
class DisplaceModel;

class OutputFileParser
{
    MainWindow *mOwner;
public:
    OutputFileParser(MainWindow *owner);

    void parse (QString path, DisplaceModel *model);

protected:
    void parsePopStart(QFile *file, DisplaceModel *model);
};

#endif // OUTPUTFILEPARSER_H
