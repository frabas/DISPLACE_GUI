#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>

#include <Harbour.h>

#include <QString>
#include <QList>

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

    int getHarboursCount() const;
    QString getHarbourId(int idx) const;


    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    //

    QString getLastError() const { return mLastError; }

protected:


private:
    QString mName;
    QString mBasePath;
    QString mOutputName;

    Scenario mScenario;

    QList<Harbour> mHarbours;

    // ---

    QString mLastError;
};

#endif // DISPLACEMODEL_H
