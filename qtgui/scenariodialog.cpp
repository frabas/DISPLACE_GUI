#include "scenariodialog.h"
#include "ui_scenariodialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>

static const char *dyn_alloc_options[] = {
    "baseline",
    "focus_on_high_profit_grounds" ,
    "area_closure",
    "fuelprice_plus20percent",
    "reduced_speed_10percent",
    "closer_grounds",
    "closer_port",
    "area_closure_fuelprice_plus20percent",
    "increased_catch_power_5percent",
    "focus_on_high_profit_grounds_area_closure"
};

static const char *dyn_pop_options[] = {
    "baseline",
    "use_SMS",
    "baseline_slower_growth", "lower_pop_productivity",
    "use_SMS_slower_growth","lower_pop_productivity",
    "with_monthly_redistribution",
};

ScenarioDialog::ScenarioDialog(const Scenario & scenario, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScenarioDialog),
    mScen(scenario),
    mRenamed(false)
{
    ui->setupUi(this);


    setDynPop(mScen.getDyn_pop_sce());
    setDynAlloc(mScen.getDyn_alloc_sce());
    ui->biol_sce->setText(mScen.getBiolsce());
    ui->agraph->setValue(mScen.getGraph());
    ui->aport->setValue(mScen.getA_port());
    ui->gridres->setValue(mScen.getGraph_res());
    ui->nrowcoord->setValue(mScen.getNrow_coord());
    ui->nrowgraph->setValue(mScen.getNrow_graph());

    ui->scenarioPath->setReadOnly(true);
}

ScenarioDialog::~ScenarioDialog()
{
    delete ui;
}

void ScenarioDialog::setDynPop(const QStringList &options)
{
    QGridLayout *lay = new QGridLayout();
    ui->group_dyn_pop_sce->setLayout(lay);

    int n = sizeof(dyn_pop_options) / sizeof(dyn_pop_options[0]);

    for (int i = 0; i < n; ++i) {
        QCheckBox *box = new QCheckBox(dyn_pop_options[i]);

        if (options.contains(dyn_pop_options[i]))
            box->setChecked(true);

        lay->addWidget(box, i / 3, i % 3, 1, 1);
        optPop.append(box);
    }
}

void ScenarioDialog::setDynAlloc(const QStringList &options)
{
    QGridLayout *lay = new QGridLayout();
    ui->group_dyn_alloc_sce->setLayout(lay);

    int n = sizeof(dyn_alloc_options) / sizeof(dyn_alloc_options[0]);
    for (int i = 0; i < n; ++i) {
        QCheckBox *box = new QCheckBox(dyn_alloc_options[i]);
        if (options.contains(dyn_alloc_options[i]))
            box->setChecked(true);
        lay->addWidget(box, i / 3, i % 3, 1, 1);
        optAll.append(box);
    }
}
QString ScenarioDialog::getScenarioPath() const
{
    return mScenarioPath;
}

void ScenarioDialog::setScenarioPath(const QString &value)
{
    mScenarioPath = value;
    ui->scenarioPath->setText(mScenarioPath);
    ui->scenarioPath->setToolTip(mScenarioPath);
}


void ScenarioDialog::on_ScenarioDialog_accepted()
{
    QStringList optionsPop;
    foreach (QCheckBox *box, optPop) {
        if (box->isChecked()) {
            optionsPop << box->text();
        }
    }

    mScen.setDyn_pop_sce(optionsPop);

    QStringList optionsAlloc;
    foreach (QCheckBox *box, optAll) {
        if (box->isChecked()) {
            optionsAlloc << box->text();
        }
    }

    mScen.setDyn_alloc_sce(optionsAlloc);

    mScen.setBiolsce(ui->biol_sce->text());
    mScen.setGraph(ui->agraph->value());
    mScen.setA_port(ui->aport->value());
    mScen.setGraph_res(ui->gridres->value());
    mScen.setNrow_coord(ui->nrowcoord->value());
    mScen.setNrow_graph(ui->nrowgraph->value());
}

void ScenarioDialog::on_rename_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("lastpath", QDir::homePath()).toString();
    QString s = QFileDialog::getSaveFileName(this, tr("Enter Scenario Filename"),
                                             lastpath, QString("*.dat"));
    if (!s.isEmpty()) {
        QFileInfo info(s);
        if (info.suffix() != "dat") {
            s += ".dat";
        }

        setScenarioPath(s);
        sets.setValue("lastpath", s);
        mRenamed = true;
    }
}
