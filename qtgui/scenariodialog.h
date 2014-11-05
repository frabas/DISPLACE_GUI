#ifndef SCENARIODIALOG_H
#define SCENARIODIALOG_H

#include <QDialog>
#include <scenario.h>

namespace Ui {
class ScenarioDialog;
}

QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

class ScenarioDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScenarioDialog(const Scenario & scenario, QWidget *parent = 0);
    ~ScenarioDialog();

    const Scenario &getScenario() const { return mScen; }

private slots:
    void on_ScenarioDialog_accepted();

private:
    void setDynPop(const QStringList &text);
    void setDynAlloc(const QStringList &options);

private:
    Ui::ScenarioDialog *ui;

    Scenario mScen;
    QList<QCheckBox *> optPop;
    QList<QCheckBox *> optAll;
};

#endif // SCENARIODIALOG_H
