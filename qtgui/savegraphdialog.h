#ifndef SAVEGRAPHDIALOG_H
#define SAVEGRAPHDIALOG_H

#include <QDialog>

namespace Ui {
class SaveGraphDialog;
}

class SaveGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveGraphDialog(QWidget *parent = 0);
    ~SaveGraphDialog();

private slots:
    void on_optCustomize_toggled(bool checked);

    void on_graphName_textChanged(const QString &arg1);

    void on_cmdBrowse_clicked();

private:
    Ui::SaveGraphDialog *ui;
};

#endif // SAVEGRAPHDIALOG_H
