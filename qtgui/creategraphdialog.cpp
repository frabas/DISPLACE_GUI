#include "creategraphdialog.h"
#include "ui_creategraphdialog.h"

#include <QMessageBox>

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

GraphBuilder::Type CreateGraphDialog::getType() const
{
    if (ui->quad->isChecked())
        return GraphBuilder::Quad;
    else
        return GraphBuilder::Hex;
}

void CreateGraphDialog::setShapefileList(QStringList list)
{
    ui->shapefileIncluding->addItems(list);
    ui->shapefileExcluding->addItems(list);
}

QString CreateGraphDialog::getIncludingSelectedShapefile() const
{
    return ui->shapefileIncluding->currentText();
}

QString CreateGraphDialog::getExcludingSelectedShapefile() const
{
    return ui->shapefileExcluding->currentText();
}

void CreateGraphDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        if (std::abs(ui->lat1->value() - ui->lat2->value()) < 1e-5 ||
                std::abs(ui->long1->value() - ui->long2->value()) < 1e-5 ||
            ui->distance->value() < 0.5) {
            QMessageBox::warning(this, tr("Invalid values"), tr("The fields contain some invalid value."));
        } else {
            QDialog::done(r);
        }

    } else {
        QDialog::done(r);
    }
}
