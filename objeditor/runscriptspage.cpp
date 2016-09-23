#include "runscriptspage.h"
#include "ui_runscriptspage.h"

#include <R/env.h>

#include <QProcess>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

RunScriptsPage::RunScriptsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunScriptsPage)
{
    ui->setupUi(this);

    ui->buttonBox->setLayout(mLayout = new QGridLayout);
}

RunScriptsPage::~RunScriptsPage()
{
    delete ui;
}

QPushButton * RunScriptsPage::addScriptButton(const QString &label, const QString &script, FeedArgsFunction feed_args_function, ButtonPushedFunction onButtonPushed)
{
    QPushButton *button = new QPushButton(label);

    connect (button, &QPushButton::clicked, [onButtonPushed, button, script, this, feed_args_function]() {
        QStringList args;
        if (feed_args_function != nullptr)
            feed_args_function(args);
        runScript(script, args);
        if (onButtonPushed != nullptr)
            onButtonPushed(button);
    });

    mLayout->addWidget(button, mNw / 2, mNw % 2);
    ++mNw;

    return button;
}

void RunScriptsPage::runScript(const QString &script, const QStringList &extra_args)
{
    displace::R::Env env;

    mProcess = new QProcess;

    connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(processExit(int)));

    QStringList args;
    args << script << extra_args;

    mProcess->setEnvironment(env.environment().toStringList());
    mProcess->setWorkingDirectory(env.getRScriptHome());
    mProcess->start(env.getRScriptExe(), args);

    qDebug() << "START:" << env.getRScriptExe() << args;

//    return true;

}

void RunScriptsPage::processStarted()
{
    ui->log->clear();
//    ui->run->setDisabled(true);
}

void RunScriptsPage::readOutput()
{
    QString t = mProcess->readAllStandardOutput();
    ui->log->appendPlainText(t + "\n");
}

void RunScriptsPage::readError()
{
    QString t = mProcess->readAllStandardError();
    ui->log->appendHtml("<font color=\"#aa0000\">" + t + "</font>");
}

void RunScriptsPage::processExit(int result)
{
//    ui->run->setEnabled(true);
    qDebug() << "Completed: " << result;
}

void RunScriptsPage::checkEnv()
{
    displace::R::Env env;
    if (!env.check()) {
        QMessageBox::warning(this, tr("Vessel Editor setup check"),
                             tr("Couldn't start Rscript. Please setup the Rscript path properly in the Settings screen."));
    }
}
