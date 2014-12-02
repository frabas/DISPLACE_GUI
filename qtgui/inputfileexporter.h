#ifndef INPUTFILEEXPORTER_H
#define INPUTFILEEXPORTER_H

#include <QString>

class DisplaceModel;

class InputFileExporter
{
public:
    InputFileExporter();

    bool exportGraph(QString graphpath, QString coordspath, DisplaceModel *currentModel, QString *error);
};

#endif // INPUTFILEEXPORTER_H
