#ifndef CREATEGRAPHDIALOG_H
#define CREATEGRAPHDIALOG_H

#include <QDialog>

namespace Ui {
class CreateGraphDialog;
}

class CreateGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGraphDialog(QWidget *parent = 0);
    ~CreateGraphDialog();

private:
    Ui::CreateGraphDialog *ui;
};

#endif // CREATEGRAPHDIALOG_H
