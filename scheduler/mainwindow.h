#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>
#include <schedulerjob.h>
#include <schedulerjobadapter.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    std::unique_ptr<SchedulerJob> mScheduler;
    SchedulerJobAdapter *mAdapter;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Add_triggered();

    void on_action_Save_triggered();

    void on_action_Open_triggered();

    void on_action_Generate_Script_triggered();

    void on_action_Remove_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
