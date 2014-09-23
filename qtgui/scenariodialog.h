#ifndef SCENARIODIALOG_H
#define SCENARIODIALOG_H

#include <QDialog>
#include <scenario.h>

namespace Ui {
class ScenarioDialog;
}

class ScenarioDialog : public QDialog
{
    Q_OBJECT

    Scenario mScen;
public:
    explicit ScenarioDialog(const Scenario & scenario, QWidget *parent = 0);
    ~ScenarioDialog();

    const Scenario &getScenario() const { return mScen; }
private:
    Ui::ScenarioDialog *ui;
};

#endif // SCENARIODIALOG_H
