#ifndef SCRIPTSELECTIONFORM_H
#define SCRIPTSELECTIONFORM_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

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
    void on_reset_clicked();
    void on_ScriptSelectionForm_accepted();
    void on_browseBasePath_clicked();

private:
    Ui::ScriptSelectionForm *ui;

    QGridLayout *mGrid;
    void addDelegate(const QString &label, const QString &key);
};

#endif // SCRIPTSELECTIONFORM_H
