#ifndef VESSELEDITORMAINWINDOW_H
#define VESSELEDITORMAINWINDOW_H

#include <QMainWindow>

#include <memory>

namespace Ui {
class VesselEditorMainWindow;
}

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QProcess;
QT_END_NAMESPACE

class VesselsSpec;
class VesselsSpecModel;

class VesselEditorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VesselEditorMainWindow(QWidget *parent = 0);
    ~VesselEditorMainWindow();

private slots:
    void on_action_Load_Vessels_Spec_triggered();
    void on_run_clicked();

    void processStarted();
    void readOutput();
    void readError();
    void processExit(int);

    void on_actionRscript_location_triggered();

    void on_actionScripts_location_triggered();

    void on_browseShapefilePath_clicked();

    void on_browseHarboursPath_clicked();

    void on_browseOutputPath_clicked();

private:
    Ui::VesselEditorMainWindow *ui;

    std::shared_ptr<VesselsSpec> mVesselsSpec;
    std::shared_ptr<VesselsSpecModel> mVesselsSpecModel;
    QSortFilterProxyModel *mVesselsSpecProxyModel;

    QProcess *mProcess;

    void checkEnv();
};

#endif // VESSELEDITORMAINWINDOW_H
