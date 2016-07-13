#ifndef VESSELEDITORMAINWINDOW_H
#define VESSELEDITORMAINWINDOW_H

#include <QMainWindow>

#include <memory>

namespace Ui {
class VesselEditorMainWindow;
}

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
QT_END_NAMESPACE

class VesselsSpec;
class VesselsSpecModel;

class VesselEditorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VesselEditorMainWindow(QWidget *parent = 0);
    ~VesselEditorMainWindow();

private slots:
    void on_action_Load_Vessels_Spec_triggered();

private:
    Ui::VesselEditorMainWindow *ui;

    std::shared_ptr<VesselsSpec> mVesselsSpec;
    std::shared_ptr<VesselsSpecModel> mVesselsSpecModel;
    QSortFilterProxyModel *mVesselsSpecProxyModel;
};

#endif // VESSELEDITORMAINWINDOW_H
