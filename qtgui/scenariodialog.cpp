#include "scenariodialog.h"
#include "ui_scenariodialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>

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
    "use_SMS_single",
    "baseline_slower_growth", "lower_pop_productivity",
    "use_SMS_slower_growth","lower_pop_productivity",
    "with_monthly_redistribution",
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
    ui->agraph->setValue(mScen.getGraph());
    ui->aport->setValue(mScen.getA_port());
    ui->gridres->setValue(mScen.getGraph_res());
    ui->isvesselquotas->setValue(mScen.getIs_individual_vessel_quotas());
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
    mScen.setIs_individual_vessel_quotas(ui->isvesselquotas->value());
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

    /** \bug the following line seems to trigger a Qt5 bug. The default filter is not selected as expected. */
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
