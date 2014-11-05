#ifndef OUTPUTFILEPARSER_H
#define OUTPUTFILEPARSER_H

#include <QObject>
#include <QString>
#include <QFile>

#include <memory>

class DisplaceModel;
class VesselStats;

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
    void parsePopStart(QFile *file, int tstep, DisplaceModel *model);
    void parsePopCumftime(QFile *file, int tstep, DisplaceModel *model);
    void parsePopImpact(QFile *file, int tstep, DisplaceModel *model);
    void parsePopdynF(QFile *file, int tstep, DisplaceModel *model);
    void parsePopdyn(QFile *file, int tstep, DisplaceModel *model);
    void parseVessels(QFile *file, int tstep, DisplaceModel *model);

    static int toInt(const QString x) {
        bool b;
        int r = x.toInt(&b);
        if (!b)
            throw std::exception();
        return r;
    }

    static double toDouble(const QString x) {
        bool b;
        double r = x.toDouble(&b);
        if (!b)
            throw std::exception();
        return r;
    }

public:
    static VesselStats parseVesselStatLine(const QStringList &fields);
};

#endif // OUTPUTFILEPARSER_H
