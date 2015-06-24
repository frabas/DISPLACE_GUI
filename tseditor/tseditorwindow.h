#ifndef TSEDITORWINDOW_H
#define TSEDITORWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTemporaryFile>
#include <QModelIndex>

#include <memory>

namespace Ui {
class TsEditorWindow;
}

class QLabel;
class CsvTableModel;

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

signals:
    void dataDirty();

public slots:
    void readOutput();
    void readError();
    void processExit(int);
    void dataChanged(QModelIndex from, QModelIndex to, QVector<int> roles);
    void dataDirtyChanged();

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

    void load(QString filename);
    void updateKeys();
    void genSampleFile();
    void loadSampleFileGraph(QString name);

    void generate(QString param_file, QString dest, QString variable, QString area, QString adim);
};

#endif // TSEDITORWINDOW_H
