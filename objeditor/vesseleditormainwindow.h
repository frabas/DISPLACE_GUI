#ifndef VESSELEDITORMAINWINDOW_H
#define VESSELEDITORMAINWINDOW_H

#include <QMainWindow>

#include <memory>

namespace Ui {
class VesselEditorMainWindow;
}

class CsvTableModel;

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
QT_END_NAMESPACE

class MapListAdapter;

class VesselEditorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VesselEditorMainWindow(QWidget *parent = 0);
    ~VesselEditorMainWindow();

    static const QString VesselsSpecFilename;
    static const QString Pop1SpecFilename;
    static const QString Pop2SpecFilename;
    static const QString Pop3SpecFilename;

private slots:
    void on_browseInputPath_clicked();
    void on_browseGISPath_clicked();
    void on_browseBasePath_clicked();
    void on_tabWidget_currentChanged(int index);

private:
    Ui::VesselEditorMainWindow *ui;
    MapListAdapter *mVesMapListAdapter;
    MapListAdapter *mPopMapListAdapter;

    bool runScript(QString script);
    void fillRScriptsArgs(QStringList &args);
};

#endif // VESSELEDITORMAINWINDOW_H
