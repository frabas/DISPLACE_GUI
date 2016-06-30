#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

VesselEditorMainWindow::VesselEditorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VesselEditorMainWindow)
{
    ui->setupUi(this);
}

VesselEditorMainWindow::~VesselEditorMainWindow()
{
    delete ui;
}
