#ifndef VESSELEDITORMAINWINDOW_H
#define VESSELEDITORMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class VesselEditorMainWindow;
}

class VesselEditorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VesselEditorMainWindow(QWidget *parent = 0);
    ~VesselEditorMainWindow();

private:
    Ui::VesselEditorMainWindow *ui;
};

#endif // VESSELEDITORMAINWINDOW_H
