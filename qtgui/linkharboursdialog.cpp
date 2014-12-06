#include "linkharboursdialog.h"
#include "ui_linkharboursdialog.h"

LinkHarboursDialog::LinkHarboursDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkHarboursDialog)
{
    ui->setupUi(this);
}

LinkHarboursDialog::~LinkHarboursDialog()
{
    delete ui;
}

bool LinkHarboursDialog::isRemoveLinksSet() const
{
    return ui->removeOldLinks->isChecked();
}

double LinkHarboursDialog::getMaxDinstance() const
{
    return ui->maxDinstance->value();
}

int LinkHarboursDialog::getMaxLinks() const
{
    return ui->maxConnections->value();
}

bool LinkHarboursDialog::isAvoidLonelyHarboursSet() const
{
    return ui->checkLonelyHarbours->isChecked();
}

bool LinkHarboursDialog::isAvoidHHLinks() const
{
    return ui->checkHarboursLinks->isChecked();
}
