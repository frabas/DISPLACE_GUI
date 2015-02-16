#include "pathpenaltydialog.h"
#include "ui_pathpenaltydialog.h"

PathPenaltyDialog::PathPenaltyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathPenaltyDialog)
{
    ui->setupUi(this);

    on_shapefile_currentIndexChanged(0);
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

bool PathPenaltyDialog::isClosedForFishing()
{
    return ui->optClosedForFishing->isChecked();
}

double PathPenaltyDialog::weight() const
{
    return ui->weight->value();
}

void PathPenaltyDialog::on_ok_clicked()
{
    if (ui->shapefile->isVisible() && ui->shapefile->currentIndex() == -1)
        return;

    accept();
}

void PathPenaltyDialog::on_shapefile_currentIndexChanged(int)
{
    ui->ok->setEnabled(!ui->shapefile->isVisible() || ui->shapefile->currentIndex() != -1);
}
