#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <vesselsspec.h>

#include <fstream>

#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>

VesselEditorMainWindow::VesselEditorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VesselEditorMainWindow)
{
    ui->setupUi(this);

    mVesselsSpec = std::make_shared<VesselsSpec>();
}

VesselEditorMainWindow::~VesselEditorMainWindow()
{
    delete ui;
}

void VesselEditorMainWindow::on_action_Load_Vessels_Spec_triggered()
{
    QSettings sets;
    QString lastpath;

    lastpath = sets.value("vessel_specs_lastpath", QDir::homePath()).toString();

    QString fn = QFileDialog::getOpenFileName(this, tr("Get Vessel Spec File"), lastpath);

    if (!fn.isEmpty()) {
        QFileInfo d (fn);
        sets.setValue("vessel_specs_lastpath", d.absoluteFilePath());

        std::ifstream f;

        f.open(fn.toStdString(), std::ios_base::in);
        if (f.fail()) {
            return;
        }

        if (!mVesselsSpec->loadFromSpecFile(f)) {
            return;
        }

        f.close();
    }
}
