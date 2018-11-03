#ifndef SCHEDULERSCRIPTGENERATOR_H
#define SCHEDULERSCRIPTGENERATOR_H

#include <QString>
#include <QFile>
#include <QStringList>

class SchedulerJob;
class SimulationRun;

class SchedulerScriptGenerator
{
    QString mTemplateName;
public:
    explicit SchedulerScriptGenerator(const QString &templatename);
    virtual ~SchedulerScriptGenerator() noexcept;

    virtual bool generate (QString path, SchedulerJob *scheduler, QString *errorMsg);

private:
    void parseTemplate(QString templatePath);
    QString substKeywords(QString in);
    QString substKeywords(QString in, const SimulationRun &job);

    bool getValue(QString key, QString &value);
    bool getValue(QString key, const SimulationRun &job, QString &value);

    QStringList mHeaders, mContent, mFooters;

    static const QString sBeginHeader, sEndHeader, sBeginContent, sEndContent, sBeginFooter, sEndFooter;

    static const QString sKeyAppPath;
    static const QString sKeyAppExec;

    static const QString sKeyJobOutDir;
    static const QString sKeyJobName;
    static const QString sKeyJobPath;
    static const QString sKeySimuName;
    static const QString sKeySimuOutName;
    static const QString sKeySimuLength;
    static const QString sKeySimuNumThreads;

    static const QString sValueAppExec;
};

#endif // SCHEDULERSCRIPTGENERATOR_H
