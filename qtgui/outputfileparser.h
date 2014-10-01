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
    void parse (QString path);

signals:
    void error(QString);

protected:
    void parsePopStart(QFile *file, DisplaceModel *model);
};

#endif // OUTPUTFILEPARSER_H
