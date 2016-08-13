#ifndef SCRIPTSELECTIONFORM_H
#define SCRIPTSELECTIONFORM_H

#include <QDialog>

namespace Ui {
class ScriptSelectionForm;
}

class ScriptSelectionForm : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptSelectionForm(QWidget *parent = 0);
    ~ScriptSelectionForm();

private slots:
    void on_browseConfig_clicked();
    void on_browseRun_clicked();
    void on_reset_clicked();
    void on_ScriptSelectionForm_accepted();

private:
    Ui::ScriptSelectionForm *ui;
};

#endif // SCRIPTSELECTIONFORM_H
