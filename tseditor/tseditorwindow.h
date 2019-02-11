// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#ifndef TSEDITORWINDOW_H
#define TSEDITORWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTemporaryFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QModelIndex>

#include <memory>

namespace Ui {
class TsEditorWindow;
}

class QLabel;
class CsvTableModel;
class QProgressDialog;

class TsEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TsEditorWindow(QWidget *parent = 0);
    ~TsEditorWindow();

private slots:
    void on_action_Open_triggered();
    void on_varSelect_currentIndexChanged(const QString &arg1);
    void on_areaSelect_activated(const QString &arg1);
    void on_adimSelect_activated(const QString &arg1);
    void on_action_Log_Window_triggered();
    void on_clearLog_clicked();
    void on_dockLogWindow_visibilityChanged(bool visible);
    void on_actionQuit_triggered();
    void on_action_Save_triggered();
    void on_actionGenerate_triggered();
    void on_actionRScript_location_triggered();
    void on_actionGen_Script_location_triggered();

signals:
    void dataDirty();
    void exportProgress(int value);
    void exportTotalChanged(int total);

public slots:
    void processStarted();
    void readOutput();
    void readError();
    void processExit(int);
    void dataChanged(QModelIndex from, QModelIndex to, QVector<int> roles);
    void dataDirtyChanged();
    void exportFinished();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::TsEditorWindow *ui;

    QString mFilename;
    std::shared_ptr<QList<QStringList> > mData;
    CsvTableModel *mModel;

    int colVar, colArea, colADim;

    QProcess *mProcess;
    QTemporaryFile mDestFile, mParFile;

    bool mDirty;
    QLabel *mDirtyIndicator;

    QFuture<QString> mExportWorker;
    QFutureWatcher<QString> mExportWorkerWatcher;
    QProgressDialog *mExporterWorkerDialog;

    void checkEnv();

    QString getScriptPath();

    void load(QString filename);
    void updateKeys();
    void genSampleFile();
    void loadSampleFileGraph(QString name);

    void saveTempParamFile();
    void generate(QString param_file, QString dest, QString variable, QString area, QString adim);
    void generateAll (QString outpath);
    QString generateAllWorker(QString outpath);

    double parseThreshold(QString l);
};

#endif // TSEDITORWINDOW_H
