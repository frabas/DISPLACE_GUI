#ifndef OUTPUTFILEPARSER_H
#define OUTPUTFILEPARSER_H

#include <QObject>
#include <QString>
#include <QFile>

class DisplaceModel;

class OutputFileParser : public QObject
{
    Q_OBJECT

    DisplaceModel *mModel;

public:
    explicit OutputFileParser(DisplaceModel *model, QObject *parent = 0);

public slots:
    void parse (QString path, int tstep);

signals:
    void error(QString);
    void parseCompleted();

protected:
    void parsePopStart(QFile *file, DisplaceModel *model);
    void parsePopCumftime(QFile *file, int tstep, DisplaceModel *model);
    void parsePopdynF(QFile *file, int tstep, DisplaceModel *model);
    void parsePopdyn(QFile *file, int tstep, DisplaceModel *model);
};

#endif // OUTPUTFILEPARSER_H
