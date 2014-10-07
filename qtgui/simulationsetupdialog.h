#ifndef SIMULATIONSETUPDIALOG_H
#define SIMULATIONSETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SimulationSetupDialog;
}

class SimulationSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationSetupDialog(QWidget *parent = 0);
    ~SimulationSetupDialog();

    void setSimulationSteps(int steps);
    int getSimulationSteps() const;

    void setSimulationOutputName(QString name);
    QString getSimulationOutputName() const;

    void setSimulationName(QString name);
    QString getSimulationName() const;

    void setMoveVesselsOption(bool x);
    bool getMoveVesselsOption() const;
private:
    Ui::SimulationSetupDialog *ui;
};

#endif // SIMULATIONSETUPDIALOG_H
