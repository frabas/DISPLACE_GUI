#ifndef CSVEDITOR_H
#define CSVEDITOR_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

#include <utils/csvtablemodel.h>
#include <waitdialog.h>
#include <utils/csvimporter.h>

#include <memory>

namespace Ui {
class CsvEditor;
}

class CsvEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit CsvEditor(QWidget *parent = 0);
    ~CsvEditor();

private slots:
    void onLoadFinished();

    void on_action_Open_triggered();
    void on_actionFirst_line_as_Headers_toggled(bool);

    void on_action_Save_triggered();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::CsvEditor *ui;
    CsvTableModel *mModel;
    std::shared_ptr<QList<QStringList> > mData;
    QString mFilename;

    QFuture<QList<QStringList>> mWorkLoading;
    QFutureWatcher<QList<QStringList>> mWorkLoadingWatcher;
    WaitDialog *mWaitDialog;
    CsvImporter mImporter;

    void updateCheckState(bool state);
};

#endif // CSVEDITOR_H
