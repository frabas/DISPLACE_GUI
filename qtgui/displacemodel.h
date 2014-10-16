#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>
#include <config.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <modelobjects/benthos.h>
#include <modelobjects/populationdata.h>
#include <modelobjects/nationdata.h>
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
    typedef QVector<NationStats> NationsStats;
    typedef HistoricalDataCollector<NationsStats> NationsStatsContainer;

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

    /* Access to Population statistics */
    int getPopulationsCount() const;
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

    /* Access to Nations statistics */

    const QList<NationData> &getNationsList() const { return mNations; }
    const NationData &getNation(int idx) const { return mNations.at(idx); }

    int getNationsStatsCount() const {
        return mStatsNations.getUniqueValuesCount();
    }
    NationsStatsContainer::Container::const_iterator getNationsStatsFirstValue() const {
        return mStatsNations.getFirst();
    }
    const NationsStats &getNationsStatAtStep(int step) const {
        return mStatsNations.getValue(step);
    }
    const NationStats &getNationStatAtStep(int step, int idx) const {
        return mStatsNations.getValue(step).at(idx);
    }

    /* Scenario and configuration */

    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    Config config() const;
    void setConfig(const Config &config);

    /* Live Simulation data */

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
    void clearInterestingPop();

    /* Interesting pop access functions */
    bool isInterestingSizeTotal() const { return mInterestingSizeTotal; }
    void setInterestingSizeTotal(bool b) { mInterestingSizeTotal = b; }

    const QList<int> &getInterestingSizes() const { return mInterestingSizes; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingSize(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingSize(int n);
    bool isInterestingSize(int n);

    /* Interesting harbours - see pop */
    const QList<int> &getInterestingHarbours() const { return mInterestingHarb; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingHarb(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingHarb(int n);
    bool isInterestingHarb(int n);

    /* Interesting Nations */
    const QList<int> &getInterestingNations() const { return mInterestingNations; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingNations(int n) {
        if (!mInterestingNations.contains(n))
            mInterestingNations.append(n);
            qSort(mInterestingNations);
    }

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingNations(int n) {
        mInterestingNations.removeAll(n);
    }
    bool isInterestingNations(int n) {
        return mInterestingNations.contains(n);
    }

    //

    QString getLastError() const { return mLastError; }

    void parseOutputStatsFile (QString file, int tstep);

    void commitNodesStatsFromSimu(int tstep);

    // Stats collecting. Must store data to db
    void startCollectingStats();
    void endCollectingStats();
    void collectNodePopStats (int tstep, int node_idx, const QList<double> &stats, const QList<double> &stats_w, double tot, double wtot);
    void collectPopCumftime(int step, int node_idx, double cumftime);
    void collectPopImpact(int step, int node_idx, int popid, double impact);

    void collectPopdynN(int step, int popid, const QVector<double> &pops, double value);
    void collectPopdynF(int step, int popid, const QVector<double> &pops, double value);

    void collectVesselStats (int step, std::shared_ptr<VesselStats> stats);
protected:
    bool loadNodes();
    bool loadVessels();
    bool initBenthos();
    bool initPopulations();
    bool initNations();

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
    bool mVesselsStatsDirty;

    bool mLive;
    Scenario mScenario;
    Config mConfig;

    QList<int> mInterestingPop;
    bool mInterestingSizeTotal;
    QList<int> mInterestingSizes;
    QList<int> mInterestingHarb;
    QList<int> mInterestingNations;

    QList<Harbour *> mHarbours;
    QList<NodeData *> mNodes;
    QList<VesselData *> mVessels;
    QList<Benthos *> mBenthos;
    QList<NationData> mNations;

    PopulationStatContainer mStatsPopulations;
    PopulationStat mStatsPopulationsCollected;
    NationsStatsContainer mStatsNations;
    NationsStats mStatsNationsCollected;

    QMap<int, Benthos *> mBenthosInfo;

    // --- Working objects

    OutputFileParser *mOutputFileParser;
    QThread *mParserThread;

    QString mLastError;
};

#endif // DISPLACEMODEL_H
