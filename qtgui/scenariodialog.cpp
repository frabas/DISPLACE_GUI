#include "scenariodialog.h"
#include "ui_scenariodialog.h"

ScenarioDialog::ScenarioDialog(const Scenario & scenario, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScenarioDialog),
    mScen(scenario)
{
    ui->setupUi(this);

    ui->dyn_alloc_sce->setText(mScen.getDyn_alloc_sce().join(" "));
    ui->dyn_pop_sce->setText(mScen.getDyn_pop_sce().join(" "));
    ui->biol_sce->setText(mScen.getBiolsce());
    ui->agraph->setValue(mScen.getGraph());
    ui->aport->setValue(mScen.getA_port());
    ui->gridres->setValue(mScen.getGraph_res());
    ui->nrowcoord->setValue(mScen.getNrow_coord());
    ui->nrowgraph->setValue(mScen.getNrow_graph());
}

ScenarioDialog::~ScenarioDialog()
{
    delete ui;
}
