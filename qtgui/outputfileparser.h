#ifndef OUTPUTFILEPARSER_H
#define OUTPUTFILEPARSER_H

#include <QString>

class MainWindow;

class OutputFileParser
{
    MainWindow *mOwner;
public:
    OutputFileParser(MainWindow *owner);

    void parse (QString path);

protected:
    void parsePopStart(QFile file);
};

#endif // OUTPUTFILEPARSER_H
