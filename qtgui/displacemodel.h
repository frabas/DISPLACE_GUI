#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>
#include <config.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <modelobjects/benthos.h>
#include <Harbour.h>
#include <outputfileparser.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QThread>

class DbHelper;

class DisplaceModel : public QObject
{
    Q_OBJECT
public:
    DisplaceModel();

    bool load (QString path, QString modelname, QString outputname);
    bool loadDatabase (QString path);
    bool linkDatabase (QString path);
    bool save();

    /** \brief Signals the simulation has ended. Flush all buffers */
    void simulationEnded();

    // Getter
    QString name() const { return mName; }
    QString basepath() const { return mBasePath; }
    QString outputName() const { return mOutputName; }
    bool isModelLoaded() const { return !mName.isEmpty(); }

    const QList<Harbour *> &getHarboursList() const { return mHarbours; }
    int getHarboursCount() const;
    QString getHarbourId(int idx) const;

    const QList<NodeData *> &getNodesList() const { return mNodes; }
    int getNodesCount() const;
    QString getNodeId(int idx) const;

    /** \brief receive a Stats update for nodes from the Simulator
     *
     * The string passed has the following format:
     *    stat,tstep,first,number,data
     *
     * */
    void updateNodesStatFromSimu(QString);

    const QList<VesselData *> &getVesselList() const { return mVessels; }
    int getVesselCount() const;
    QString getVesselId(int idx) const;
    void updateVessel (int tstep, int idx, float x, float y, float course, float fuel, int state );

    const QList<Benthos*> &getBenthosList() const { return mBenthos; }
    int getBenthosCount() const;


    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    void setCurrentStep(int step);
    int getCurrentStep() const { return mCurrentStep; }
    int getLastStep() const { return mLastStep; }

    //

    QString getLastError() const { return mLastError; }

    void parseOutputStatsFile (QString file) {
        emit parseOutput(file);
    }

    void commitNodesStatsFromSimu(int tstep);
protected:
    bool loadNodes();
    bool loadVessels();
    bool initBenthos();

    bool loadNodesFromDb();
    bool loadVesselsFromDb();

signals:
    void parseOutput(QString);
    void errorParsingStatsFile(QString);

private:
    DbHelper *mDb;
    QString mName;
    QString mBasePath;
    QString mOutputName;

    int mCurrentStep, mLastStep;
    int mLastStats;
    bool mNodesStatsDirty;

    bool mLive;
    Scenario mScenario;
    Config mConfig;

    QList<Harbour *> mHarbours;
    QList<NodeData *> mNodes;
    QList<VesselData *> mVessels;
    QList<Benthos *> mBenthos;
    QMap<int, Benthos *> mBenthosInfo;

    // --- Working objects

    OutputFileParser *mOutputFileParser;
    QThread *mParserThread;

    QString mLastError;
};

#endif // DISPLACEMODEL_H
