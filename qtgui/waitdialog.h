#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>

namespace Ui {
class WaitDialog;
}

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = 0);
    ~WaitDialog();


    void setText(QString text);
    void setProgress(bool shown = false, int max = 10);

public slots:
    void setProgression (int level);

private:
    Ui::WaitDialog *ui;
};

#endif // WAITDIALOG_H
