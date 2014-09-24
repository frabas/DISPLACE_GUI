#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>
#include <config.h>

#include <Node.h>
#include <Harbour.h>

#include <QString>
#include <QList>

class HarbourMapObject;

class DisplaceModel
{
public:
    DisplaceModel();

    bool load (QString path, QString modelname, QString outputname);
    bool save();

    // Getter
    QString name() const { return mName; }
    QString basepath() const { return mBasePath; }
    QString outputName() const { return mOutputName; }
    bool isModelLoaded() const { return !mName.isEmpty(); }

    const QList<Harbour *> &getHarboursList() const { return mHarbours; }
    int getHarboursCount() const;
    QString getHarbourId(int idx) const;

    const QList<Node *> &getNodesList() const { return mNodes; }
    int getNodesCount() const;
    QString getNodeId(int idx) const;

    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    //

    QString getLastError() const { return mLastError; }

protected:
    bool loadNodes();

private:
    QString mName;
    QString mBasePath;
    QString mOutputName;

    Scenario mScenario;
    Config mConfig;

    QList<Harbour *> mHarbours;
    QList<Node *> mNodes;
    // ---

    QString mLastError;
};

#endif // DISPLACEMODEL_H
