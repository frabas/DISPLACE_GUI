#include "pathpenaltydialog.h"
#include "ui_pathpenaltydialog.h"

PathPenaltyDialog::PathPenaltyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathPenaltyDialog)
{
    ui->setupUi(this);
}

PathPenaltyDialog::~PathPenaltyDialog()
{
    delete ui;
}

void PathPenaltyDialog::showShapefileOptions(bool show)
{
    ui->shapefile->setVisible(show);
    ui->label_shapefile->setVisible(show);
}

void PathPenaltyDialog::setShapefileList(QStringList files)
{
    ui->shapefile->addItems(files);
}

QString PathPenaltyDialog::selectedShapefile() const
{
    return ui->shapefile->currentText();
}

double PathPenaltyDialog::weight() const
{
    return ui->weight->value();
}
