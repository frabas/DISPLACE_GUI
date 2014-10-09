#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>
#include <config.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <modelobjects/benthos.h>
#include <modelobjects/populationdata.h>
#include <Harbour.h>
#include <historicaldatacollector.h>
#include <outputfileparser.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QThread>

#include <memory>

#include <QDebug>

class DbHelper;

class DisplaceModel : public QObject
{
    Q_OBJECT
public:
    typedef QVector<PopulationData> PopulationStat;
    typedef HistoricalDataCollector<PopulationStat> PopulationStatContainer;


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

    int getNBPops() const {
        return mConfig.getNbpops();
    }
    int getSzGrupsCount() const {
        return mConfig.getSzGroups();
    }

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

    int getPopulationsCount() const { return numPopulations; }
    const PopulationData &getPopulationsAtStep (int step, int idx) const {
        return mStatsPopulations.getValue(step).at(idx);
    }
    int getPopulationsValuesCount() const {
        return mStatsPopulations.getUniqueValuesCount();
    }
    PopulationStatContainer::Container::const_iterator getPopulationsFirstValue() const {
        return mStatsPopulations.getFirst();
    }

    const PopulationData &getPopulations(int idx) const { return getPopulationsAtStep(mCurrentStep,idx); }

    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    void setCurrentStep(int step);
    int getCurrentStep() const { return mCurrentStep; }
    int getLastStep() const { return mLastStep; }


    /* Interesting pop access functions */
    const QList<int> &getInterestingPops() const { return mInterestingPop; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingPop(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingPop(int n);
    bool isInterestingPop(int n);

    /* Interesting harbours - see pop */
    const QList<int> &getInterestingHarbours() const { return mInterestingHarb; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingHarb(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingHarb(int n);
    bool isInterestingHarb(int n);


    //

    QString getLastError() const { return mLastError; }

    void parseOutputStatsFile (QString file, int tstep);

    void commitNodesStatsFromSimu(int tstep);

    // Stats collecting. Must store data to db
    void startCollectingStats();
    void endCollectingStats();
    void collectNodePopStats (int tstep, int node_idx, const QList<double> &stats, double tot);
    void collectPopCumftime(int step, int node_idx, double cumftime);

    void collectPopdynN(int step, int popid, double value);
    void collectPopdynF(int step, int popid, double value);


    static const int numPopulations;
protected:
    bool loadNodes();
    bool loadVessels();
    bool initBenthos();
    bool initPopulations();

    bool loadNodesFromDb();
    bool loadVesselsFromDb();
    bool loadHistoricalStatsFromDb();

    void checkStatsCollection(int tstep);

signals:
    void parseOutput(QString, int);
    void outputParsed();
    void errorParsingStatsFile(QString);

private:
    DbHelper *mDb;
    QString mName;
    QString mBasePath;
    QString mOutputName;

    int mCurrentStep, mLastStep;
    int mLastStats;
    bool mNodesStatsDirty;
    bool mPopStatsDirty;

    bool mLive;
    Scenario mScenario;
    Config mConfig;

    QList<int> mInterestingPop;
    QList<int> mInterestingHarb;

    QList<Harbour *> mHarbours;
    QList<NodeData *> mNodes;
    QList<VesselData *> mVessels;
    QList<Benthos *> mBenthos;

    PopulationStatContainer mStatsPopulations;
    PopulationStat mStatsPopulationsCollected;
    QMap<int, Benthos *> mBenthosInfo;

    // --- Working objects

    OutputFileParser *mOutputFileParser;
    QThread *mParserThread;

    QString mLastError;
};

#endif // DISPLACEMODEL_H
