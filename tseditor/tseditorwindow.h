#ifndef TSEDITORWINDOW_H
#define TSEDITORWINDOW_H

#include <QMainWindow>

#include <memory>

namespace Ui {
class TsEditorWindow;
}

class CsvTableModel;

class TsEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TsEditorWindow(QWidget *parent = 0);
    ~TsEditorWindow();

private slots:
    void on_action_Open_triggered();

private:
    Ui::TsEditorWindow *ui;

    QString mFilename;
    std::shared_ptr<QList<QStringList> > mData;
    CsvTableModel *mModel;

    int colVar, colArea, colADim;

    void load(QString filename);
    void updateKeys();
};

#endif // TSEDITORWINDOW_H
