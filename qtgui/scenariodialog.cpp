// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "scenariodialog.h"
#include "ui_scenariodialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>

#include <QDebug>

static const char *dyn_alloc_options[] = {
    "baseline",
    "area_closure",
    "area_monthly_closure",
    "closed_to_others_as_well",
    "focus_on_high_profit_grounds" ,
    "closer_grounds",
    "closer_port",
    "fuelprice_plus20percent",
    "reduced_speed_10percent",
    "shared_harbour_knowledge",
    "TACs",
    "discard_ban",
    "groupedTACs",
    "stopGoingFishingOnFirstChokedStock",
    "LTMP",
    "FMSY",
    "FMSYrange",
    "fishing_credits",
    "promote_high_tariffs",
    "TechCreeping3Per",
    "ExitVessels10Per",
    "halfShippingDensity",
    "EffortMinControl",
    "EffortMaxControl",
};

static const char *dyn_pop_options[] = {
    "baseline",
    "includeForcingLayers",
    "diffuseBenthos",
    "modelBenthosInN",
    "modeldirectKillingOnBenthos",
    "modelResuspensionEffectOnBenthos",
    "modelShippingOnBenthos",
    "diffusePopN",
    "sizeSpectra",
    "diffuseNutrients",
    "noHyperstability",
};

ScenarioDialog::ScenarioDialog(const Scenario & scenario, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScenarioDialog),
    mScen(scenario),
    mRenamed(false),
    mForceRenamed(false)
{
    ui->setupUi(this);

    setDynPop(mScen.getDyn_pop_sce());
    setDynAlloc(mScen.getDyn_alloc_sce());
    ui->biol_sce->setText(mScen.getBiolsce());
    ui->fleet_sce->setText(mScen.getFleetsce());
    ui->agraph->setValue(mScen.getGraph());
    ui->freqDoGrowth->setValue(mScen.getFreqDoGrowth());
    ui->freqDispatchThePop->setValue(mScen.getFreqDispatchThePop());
    ui->aport->setValue(mScen.getA_port().toIndex());  
    for(int i=0 ; i < mScen.getGraph_res().length() ; i++) ui->gridres->setText(mScen.getGraph_res().at(i));
    ui->individualQuotas->setChecked(mScen.getIs_individual_vessel_quotas());
    ui->checkAllStocks->setChecked(mScen.getIs_check_all_stocks_before_going_fishing());
    ui->nrowcoord->setValue(mScen.getNrow_coord());
    ui->nrowgraph->setValue(mScen.getNrow_graph());

    ui->scenarioPath->setReadOnly(true);

    qDebug() << "Go Fish" << mScen.getDtGoFishing() << mScen.getDtGoFishing().isEmpty();

    ui->dt_enable_go_fishing->setChecked(!mScen.getDtGoFishing().isEmpty());
    on_dt_enable_go_fishing_toggled(!mScen.getDtGoFishing().isEmpty());
    ui->dt_go_fishing->setText(mScen.getDtGoFishing());
    ui->dt_enable_choose_ground->setChecked(!mScen.getDtChooseGround().isEmpty());
    on_dt_enable_choose_ground_toggled(!mScen.getDtGoFishing().isEmpty());
    ui->dt_choose_ground->setText(mScen.getDtChooseGround());
    ui->dt_enable_start_fishing->setChecked(!mScen.getDtStartFishing().isEmpty());
    on_dt_enable_start_fishing_toggled(!mScen.getDtGoFishing().isEmpty());
    ui->dt_start_fishing->setText(mScen.getDtStartFishing());
    ui->dt_enable_change_ground->setChecked(!mScen.getDtChangeGround().isEmpty());
    on_dt_enable_change_ground_toggled(!mScen.getDtGoFishing().isEmpty());
    ui->dt_change_ground->setText(mScen.getDtChangeGround());
    ui->dt_enable_stop_fishing->setChecked(!mScen.getDtStopFishing().isEmpty());
    on_dt_enable_stop_fishing_toggled(!mScen.getDtGoFishing().isEmpty());
    ui->dt_stop_fishing->setText(mScen.getDtStopFishing());
    ui->dt_enable_change_port->setChecked(!mScen.getDtChangePort().isEmpty());
    on_dt_enable_change_port_toggled(!mScen.getDtGoFishing().isEmpty());
    ui->dt_change_port->setText(mScen.getDtChangePort());

    ui->useDTree->setChecked(mScen.isDtreesEnabled());
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
    mScen.setFleetsce(ui->fleet_sce->text());
    mScen.setGraph(ui->agraph->value());
    mScen.setFreqDoGrowth(ui->freqDoGrowth->value());
    mScen.setFreqDispatchThePop(ui->freqDispatchThePop->value());
    mScen.setA_port(types::NodeId(ui->aport->value()));
    mScen.setGraph_res( (QStringList)ui->gridres->text());
    mScen.setIs_individual_vessel_quotas(ui->individualQuotas->isChecked());
    mScen.setIs_check_all_stocks_before_going_fishing(ui->checkAllStocks->isChecked());
    mScen.setNrow_coord(ui->nrowcoord->value());
    mScen.setNrow_graph(ui->nrowgraph->value());

    mScen.setDtGoFishing(ui->dt_go_fishing->text());
    mScen.setDtChooseGround(ui->dt_choose_ground->text());
    mScen.setDtStartFishing(ui->dt_start_fishing->text());
    mScen.setDtChangeGround(ui->dt_change_ground->text());
    mScen.setDtStopFishing(ui->dt_stop_fishing->text());
    mScen.setDtChangePort(ui->dt_change_port->text());
    mScen.setDtreesEnabled(ui->useDTree->isChecked());
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
        sets.setValue("lastpath", info.path());
        mRenamed = true;
    }
}

void ScenarioDialog::on_apply_clicked()
{
    if (mScenarioPath.isEmpty()) {
        QMessageBox::warning(this, tr("Rename Scenario"),
                             tr("The scenario name cannot be empty."));
        return;
    }

    QRegExp regexp("(.*)/simusspe_([^/]+)/([^/]+).dat");

    if (regexp.indexIn(mScenarioPath) == -1) {
        QMessageBox::warning(this, tr("Wrong Scenario name/path"),
                             tr("The scenario name must fit the following template:\n"
                                ".../simusspe_*/*.dat"));
        return;
    }

    if (mForceRenamed && !mRenamed) {
        QMessageBox::warning(this, tr("Rename Scenario"),
                             tr("You must rename the scenario before continuing."));
        return;
    }

    accept();
}

void ScenarioDialog::on_graphBrowse_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("lastpath", QDir::homePath()).toString();
    QFileDialog dlg(this,tr("Select graph file"),lastpath);
    QStringList filt;
    filt << tr("Graph files (graph*.dat)") << tr("All files (*.*)");

    dlg.setNameFilters(filt);
    if (dlg.exec() == QDialog::Accepted) {
        QString nm = dlg.selectedFiles()[0];

        QRegExp r("graph([0-9]+).dat");
        if (r.indexIn(nm) == -1) {
            QMessageBox::warning(this, tr("Wrong Graph file name"),
                    tr("The graph file you selected cannot be parsed correctly, and the graph code cannot be retrieved."));
            return;
        }

        int nmv = r.cap(1).toInt();
        ui->agraph->setValue(nmv);

        QFileInfo info(nm);
        QString gf = info.path() + QString("/graph%1.dat").arg(nmv);

        QFile gff (gf);
        if (!gff.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Cannot read graph file name"),
                    QString(tr("The graph file you selected cannot be read: %1")).arg(gff.errorString()));
            return;
        }

        int nr = 0;
        QTextStream strm (&gff);
        while (!strm.atEnd()) {
            strm.readLine();
            ++nr;
        }
        gff.close();

        ui->nrowgraph->setValue(nr/3);

        QString cf = info.path() + QString("/coord%1.dat").arg(nmv);

        QFile cff (cf);
        if (!cff.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Cannot read coord file name"),
                    QString(tr("The coord file you selected cannot be read: %1")).arg(gff.errorString()));
            return;
        }

        nr = 0;
        QTextStream cstrm (&cff);
        while (!cstrm.atEnd()) {
            cstrm.readLine();
            ++nr;
        }
        cff.close();

        ui->nrowcoord->setValue(nr/3);
    }
}

void ScenarioDialog::on_dt_enable_go_fishing_toggled(bool checked)
{
    ui->dt_browse_go_fishing->setEnabled(checked);
    ui->dt_go_fishing->setEnabled(checked);
}


void ScenarioDialog::on_dt_enable_choose_ground_toggled(bool checked)
{
    ui->dt_browse_choose_ground->setEnabled(checked);
    ui->dt_choose_ground->setEnabled(checked);
}

void ScenarioDialog::on_dt_enable_start_fishing_toggled(bool checked)
{
    ui->dt_browse_start_fishing->setEnabled(checked);
    ui->dt_start_fishing->setEnabled(checked);
}

void ScenarioDialog::on_dt_enable_change_ground_toggled(bool checked)
{
    ui->dt_browse_change_ground->setEnabled(checked);
    ui->dt_change_ground->setEnabled(checked);
}

void ScenarioDialog::on_dt_enable_stop_fishing_toggled(bool checked)
{
    ui->dt_browse_stop_fishing->setEnabled(checked);
    ui->dt_stop_fishing->setEnabled(checked);
}

void ScenarioDialog::on_dt_enable_change_port_toggled(bool checked)
{
    ui->dt_browse_change_port->setEnabled(checked);
    ui->dt_change_port->setEnabled(checked);
}

void ScenarioDialog::browse(QString title, QLineEdit *ed)
{
    QSettings sets;
    QString lastpath = sets.value("last_dtree_path", QDir::homePath()).toString();

    QFileDialog dlg(this, title, lastpath, tr("Dat files (*.dt.csv);;All files (*)"));
    dlg.setDefaultSuffix(".dt.csv");
    if (dlg.exec() == QDialog::Accepted) {
        QFileInfo in(dlg.selectedFiles()[0]);

        ed->setText(in.fileName());

        sets.setValue("last_dtree_path", in.filePath());
    }
}

void ScenarioDialog::on_dt_browse_go_fishing_clicked()
{
    browse(tr("Go Fishing"), ui->dt_go_fishing);
}

void ScenarioDialog::on_dt_browse_choose_ground_clicked()
{
    browse(tr("Chose Ground"), ui->dt_choose_ground);
}

void ScenarioDialog::on_dt_browse_start_fishing_clicked()
{
    browse(tr("Start Fishing"), ui->dt_start_fishing);
}

void ScenarioDialog::on_dt_browse_change_ground_clicked()
{
    browse(tr("Change Ground"), ui->dt_change_ground);
}

void ScenarioDialog::on_dt_browse_stop_fishing_clicked()
{
    browse(tr("Stop Fishing"), ui->dt_stop_fishing);
}

void ScenarioDialog::on_dt_browse_change_port_clicked()
{
    browse(tr("Change Port"), ui->dt_change_port);
}
