#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include <QDialog>
#include <simulationrun.h>

namespace Ui {
class RunDialog;
}

class RunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunDialog(QWidget *parent = 0);
    ~RunDialog();

    const SimulationRun &get() const { return mData; }

private slots:
    void on_cmdBrowseModel_clicked();

    void on_cmdBatchCreate_clicked();

    void on_RunDialog_accepted();

private:
    Ui::RunDialog *ui;

    SimulationRun mData;

    bool parsePathParts(const QString &path, QString &basepath, QString &inputname, QString &outputname);
};

#endif // RUNDIALOG_H
