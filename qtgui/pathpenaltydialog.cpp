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

double PathPenaltyDialog::weight() const
{
    return ui->weight->value();
}
