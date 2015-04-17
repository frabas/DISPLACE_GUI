#ifndef MERGEDATADIALOG_H
#define MERGEDATADIALOG_H

#include <QDialog>

namespace Ui {
class MergeDataDialog;
}

/** \brief Implements the functionalities for mergedatadialog.ui
 *
 * This UI can be extended by derived classes by "plugging" additional components through the protected function insertPluggableComponent().
 * See for example \see MergePopulationDataDialog .
 * */
class MergeDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeDataDialog(QWidget *parent = 0);
    ~MergeDataDialog();

    QString getInputFile() const;
    QString getOutputFile() const;
    void setDefaultOutputToInput(bool yes) {
        mDefaultOutToIn = yes;
    }
    void setOutputRequiresTemplate(int numtemplates) {
        mOutRequiresTemplate = numtemplates;
    }

    double getDistance() const;

    QChar separator() const;

private slots:
    void on_ok_clicked();
    void on_browseIn_clicked();
    void on_browseOut_clicked();

protected:
    void insertPluggableComponent (QWidget *widget);

    virtual void inputFileSelected() {}

private:
    Ui::MergeDataDialog *ui;

    bool mDefaultOutToIn = true;
    int mOutRequiresTemplate = 0;

    static double lastDistance;
};

#endif // MERGEDATADIALOG_H
