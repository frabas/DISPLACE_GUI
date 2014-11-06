#include "creategraphdialog.h"
#include "ui_creategraphdialog.h"

CreateGraphDialog::CreateGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateGraphDialog)
{
    ui->setupUi(this);
}

CreateGraphDialog::~CreateGraphDialog()
{
    delete ui;
}

double CreateGraphDialog::step() const
{
    return ui->distance->value();
}

double CreateGraphDialog::minLon() const
{
    return ui->long1->value();
}

double CreateGraphDialog::maxLon() const
{
    return ui->long2->value();
}

double CreateGraphDialog::minLat() const
{
    return ui->lat1->value();
}

double CreateGraphDialog::maxLat() const
{
    return ui->lat2->value();
}

void CreateGraphDialog::setShapefileList(QStringList list)
{
    ui->shapefile->addItems(list);
}

QString CreateGraphDialog::getSelectedShapefile() const
{
    return ui->shapefile->currentText();
}
