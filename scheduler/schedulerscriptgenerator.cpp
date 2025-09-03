#include "schedulerscriptgenerator.h"

#include <appsettings.h>
#include <schedulerjob.h>

#include <QApplication>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>

const QString SchedulerScriptGenerator::sBeginHeader = "%%%Begin-Header%%%";
const QString SchedulerScriptGenerator::sEndHeader = "%%%End-Header%%%";
const QString SchedulerScriptGenerator::sBeginContent = "%%%Begin-Job%%%";
const QString SchedulerScriptGenerator::sEndContent = "%%%End-Job%%%";
const QString SchedulerScriptGenerator::sBeginFooter = "%%%Begin-Footer%%%";
const QString SchedulerScriptGenerator::sEndFooter = "%%%End-Footer%%%";

const QString SchedulerScriptGenerator::sKeyAppPath = "AppPath";
const QString SchedulerScriptGenerator::sKeyAppExec = "AppExec";

const QString SchedulerScriptGenerator::sKeyJobOutDir = "JobOutDir";
const QString SchedulerScriptGenerator::sKeyJobName = "JobName";
const QString SchedulerScriptGenerator::sKeyJobPath = "JobPath";
const QString SchedulerScriptGenerator::sKeySimuName = "JobSimuName";
const QString SchedulerScriptGenerator::sKeySimuOutName = "JobSimuOutName";
const QString SchedulerScriptGenerator::sKeySimuLength = "JobLength";
const QString SchedulerScriptGenerator::sKeySimuNumThreads = "JobNumThreads";
const QString SchedulerScriptGenerator::sKeyJobUseStaticPaths = "JobUseStaticPaths";


SchedulerScriptGenerator::SchedulerScriptGenerator(const QString &templatename)
    : mTemplateName(templatename)
{

}

SchedulerScriptGenerator::~SchedulerScriptGenerator() noexcept = default;

bool SchedulerScriptGenerator::generate(QString path, SchedulerJob *scheduler, QString *errorMsg)
{
    try {
        parseTemplate(mTemplateName);
    } catch (QString s) {
        if (errorMsg != nullptr)
            *errorMsg = s;
        return false;
    }

    QFile out (path);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (*errorMsg != nullptr)
            *errorMsg = out.errorString();
        return false;
    }

    QTextStream strm(&out);

    for (auto s : mHeaders) {
        strm << substKeywords(s) << "\n";
    }

    for (int i = 0; i < scheduler->jobsCount(); ++i) {
        auto &job = scheduler->job(i);

        for (auto s: mContent) {
            strm << substKeywords(s, job) << "\n";
        }
    }

    for (auto s : mFooters) {
        strm << substKeywords(s) << "\n";
    }


    out.close();

    auto perm = out.permissions();
    out.setPermissions(perm | QFile::ExeOwner | QFile::ExeGroup);

    return true;
}

void SchedulerScriptGenerator::parseTemplate(QString templatePath)
{
    enum Status { None, Header, Content, Footer } status;

    QFile f(templatePath);
    if (!f.open(QIODevice::ReadOnly))
        throw f.errorString();

    status = None;

    QTextStream strm(&f);
    while (!strm.atEnd()) {
        auto line = strm.readLine().trimmed();

        if (status == None) {
            if (line == sBeginHeader) {
                status = Header;
            } else if (line == sBeginContent) {
                status = Content;
            } else if (line == sBeginFooter) {
                status = Footer;
            }
        } else if (status == Header) {
            if (line == sEndHeader) {
                status = None;
            } else {
                mHeaders.append(line);
            }
        } else if (status == Content) {
            if (line == sEndContent) {
                status = None;
            } else {
                mContent.append(line);
            }
        } else if (status == Footer) {
            if (line == sEndFooter) {
                status = None;
            } else {
                mFooters.append(line);
            }
        }
    }

    f.close();
}

QString SchedulerScriptGenerator::substKeywords(QString in)
{
    QString ret = in;
    QRegularExpression r("%%%([a-zA-Z]+)%%%");

    auto match = r.match(in);
    if (match.hasMatch()) {
        // found a keyword.
        QString key = match.captured();
        QString value;

        if (getValue(key, value)) {
            ret = in.mid(0, match.capturedStart()) + value + in.mid(match.capturedEnd());
        }
    }

    return ret;
}

QString SchedulerScriptGenerator::substKeywords(QString in, const SimulationRun &job)
{
    QString ret = in;
    QRegularExpression r("%%%([a-zA-Z]+)%%%");

    auto match = r.match(in);
    for (int i = 0; i < match.lastCapturedIndex(); ++i) {
        QString key = match.captured(i);
        QString value;

        if (getValue(key, value) || getValue(key, job, value)) {
            ret = ret.mid(0, match.capturedStart(i)) + value + ret.mid(match.capturedEnd(i));
        }
    }

    return ret;
}

bool SchedulerScriptGenerator::getValue(QString key, QString &value)
{
    QString r;
    if (key == sKeyAppPath) {
        r = QDir::toNativeSeparators(QFileInfo(QApplication::applicationFilePath()).absolutePath());
    } else if (key == sKeyAppExec) {
        r = displace::AppSettings::getSimulatorAppName();
    }

    if (r.isEmpty())
        return false;

    value = r;
    return true;
}

bool SchedulerScriptGenerator::getValue(QString key, const SimulationRun &job, QString &value)
{
    QString r;
    if (key == sKeyJobOutDir) {
        r = job.getOutDir();
    } else if (key == sKeyJobName) {
        r = job.getName();
    } else if (key == sKeyJobPath) {
        r = QDir::toNativeSeparators(job.getPath());
    } else if (key == sKeySimuName) {
        r = job.getSimulationName();
    } else if (key == sKeySimuOutName) {
        r = job.getSimulationOutputName();
    } else if (key == sKeySimuLength) {
        r = QString::number(job.getSimulationSteps());

    } else if (key == sKeySimuNumThreads) {
        r = QString::number(job.getNumThreads());

    } else if (key == sKeyJobUseStaticPaths) {
       r = QString::number(job.getUsingStaticPaths());
    }



    if (r.isEmpty())
        return false;

    value = r;
    return true;
}




