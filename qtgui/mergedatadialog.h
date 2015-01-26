#ifndef MERGEDATADIALOG_H
#define MERGEDATADIALOG_H

#include <QDialog>

namespace Ui {
class MergeDataDialog;
}

class MergeDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeDataDialog(QWidget *parent = 0);
    ~MergeDataDialog();

    QString getInputFile() const;
    QString getOutputFile() const;
    double getDistance() const;

    QChar separator() const;
private slots:
    void on_ok_clicked();
    void on_browseIn_clicked();
    void on_browseOut_clicked();

private:
    Ui::MergeDataDialog *ui;

    static double lastDistance;
};

#endif // MERGEDATADIALOG_H
