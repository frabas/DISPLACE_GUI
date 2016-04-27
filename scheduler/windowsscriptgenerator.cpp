#include "windowsscriptgenerator.h"

#include <schedulerjob.h>

#include <QApplication>
#include <QDir>
#include <QTextStream>

const QString WindowsScriptGenerator::sBeginHeader = "%%%Begin-Header%%%";
const QString WindowsScriptGenerator::sEndHeader = "%%%End-Header%%%";
const QString WindowsScriptGenerator::sBeginContent = "%%%Begin-Job%%%";
const QString WindowsScriptGenerator::sEndContent = "%%%End-Job%%%";
const QString WindowsScriptGenerator::sBeginFooter = "%%%Begin-Footer%%%";
const QString WindowsScriptGenerator::sEndFooter = "%%%End-Footer%%%";

const QString WindowsScriptGenerator::sKeyAppPath = "AppPath";
const QString WindowsScriptGenerator::sKeyAppExec = "AppExec";

const QString WindowsScriptGenerator::sKeyJobName = "JobName";
const QString WindowsScriptGenerator::sKeyJobPath = "JobPath";
const QString WindowsScriptGenerator::sKeySimuName = "JobSimuName";
const QString WindowsScriptGenerator::sKeySimuOutName = "JobSimuOutName";
const QString WindowsScriptGenerator::sKeySimuLength = "JobLenght";
const QString WindowsScriptGenerator::sKeySimuNumThreads = "JobNumThreads";

const QString WindowsScriptGenerator::sValueAppExec = "displace.exe";

WindowsScriptGenerator::WindowsScriptGenerator()
{

}

WindowsScriptGenerator::~WindowsScriptGenerator()
{

}

bool WindowsScriptGenerator::generate(QString path, SchedulerJob *scheduler, QString *errorMsg)
{
    try {
        parseTemplate(":/scripts/windows-template");
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
        strm << substKeywords(s) << endl;
    }

    for (int i = 0; i < scheduler->jobsCount(); ++i) {
        auto &job = scheduler->job(i);

        for (auto s: mContent) {
            strm << substKeywords(s, job) << endl;
        }
    }

    for (auto s : mFooters) {
        strm << substKeywords(s) << endl;
    }


    out.close();
    return true;
}

void WindowsScriptGenerator::parseTemplate(QString templatePath) throw (QString)
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

QString WindowsScriptGenerator::substKeywords(QString in)
{
    QString ret = in;
    QRegExp r("%%%([a-zA-Z]+)%%%");

    int pos;
    if ((pos = r.indexIn(in)) != -1) {
        // found a keyword.
        QString key = r.cap(1);
        QString value;

        if (getValue(key, value)) {
            ret = in.mid(0, pos) + value + in.mid(pos + r.matchedLength());
        }
    }

    return ret;
}

QString WindowsScriptGenerator::substKeywords(QString in, const SimulationRun &job)
{
    QString ret = in;
    QRegExp r("%%%([a-zA-Z]+)%%%");

    int pos = 0;
    while ((pos = r.indexIn(ret, pos)) != -1) {
        // found a keyword.
        QString key = r.cap(1);
        QString value;

        if (getValue(key, job, value)) {
            ret = ret.mid(0, pos) + value + ret.mid(pos + r.matchedLength());
        } else if (getValue(key, value)) {
            ret = ret.mid(0, pos) + value + ret.mid(pos + r.matchedLength());
        }

        pos += value.length();
    }

    return ret;
}

bool WindowsScriptGenerator::getValue(QString key, QString &value)
{
    QString r;
    if (key == sKeyAppPath) {
        r = QDir::toNativeSeparators(QFileInfo(QApplication::applicationFilePath()).absolutePath());
    } else if (key == sKeyAppExec) {
        r = sValueAppExec;
    }

    if (r.isEmpty())
        return false;

    value = r;
    return true;
}

bool WindowsScriptGenerator::getValue(QString key, const SimulationRun &job, QString &value)
{
    QString r;
    if (key == sKeyJobName) {
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
    }

    if (r.isEmpty())
        return false;

    value = r;
    return true;
}




