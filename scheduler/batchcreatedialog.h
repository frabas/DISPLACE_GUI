#ifndef BATCHCREATEDIALOG_H
#define BATCHCREATEDIALOG_H

#include <QDialog>

namespace Ui {
class BatchCreateDialog;
}

class BatchCreateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchCreateDialog(QWidget *parent = 0);
    ~BatchCreateDialog();

    void setBaseName(QString b);

    QString getResultingName();
private slots:
    void on_first_valueChanged(int arg1);

private:
    Ui::BatchCreateDialog *ui;
};

#endif // BATCHCREATEDIALOG_H
