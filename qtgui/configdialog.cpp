#include "configdialog.h"
#include "ui_configdialog.h"

#include <displacemodel.h>

ConfigDialog::ConfigDialog(DisplaceModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog),
    mModel(model)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

bool ConfigDialog::get(Config &config) const
{
    bool ok;
    QList<int> impl;
    QStringList l = ui->m_implicit_pops->text().split(" ", QString::SkipEmptyParts);
    foreach(QString s, l) {
        impl.push_back(s.toInt(&ok));
        if (!ok)
            return false;
    }

    QList<double> cal1;
    l = ui->m_calib_oth_landings->text().split(" ", QString::SkipEmptyParts);
    foreach(QString s, l) {
        cal1.push_back(s.toDouble(&ok));
        if (!ok)
            return false;
    }

    QList<double> cal2;
    l = ui->m_calib_weight_at_szgroup->text().split(" ", QString::SkipEmptyParts);
    foreach(QString s, l) {
        cal2.push_back(s.toDouble(&ok));
        if (!ok)
            return false;
    }

    QList<double> cal3;
    l = ui->m_calib_cpue_multiplier->text().split(" ", QString::SkipEmptyParts);
    foreach(QString s, l) {
        cal3.push_back(s.toDouble(&ok));
        if (!ok)
            return false;
    }

    QList<int> ih;
    l = ui->m_int_harbours->text().split(" ", QString::SkipEmptyParts);
    foreach(QString s, l) {
        ih.push_back(s.toInt(&ok));
        if (!ok)
            return false;
    }

    config.setNbpops(ui->nbpops->value());
    config.setSzGroups(ui->nbpops->value());
    config.setImplicit_pops(impl);
    config.setCalib_oth_landings(cal1);
    config.setCalib_weight_at_szgroup(cal2);
    config.setCalib_cpue_multiplier(cal3);
    config.m_interesting_harbours = ih;

    return true;
}

void ConfigDialog::set(const Config &config)
{
    ui->nbpops->setValue(config.getNbpops());
    ui->szGroups->setValue(config.getSzGroups());

    QList<int> l1 = config.implicit_pops();
    QStringList il;
    foreach (int i, l1)
        il << QString::number(i);

    ui->m_implicit_pops->setText(il.join(" "));

    QList<double> l2 = config.calib_cpue_multiplier();
    QStringList il2;
    foreach (double i, l2)
        il2 << QString::number(i);

    ui->m_calib_cpue_multiplier->setText(il2.join(" "));

    QList<double> l3 = config.calib_oth_landings();
    QStringList il3;
    foreach (double i, l3)
        il3 << QString::number(i);

    ui->m_calib_oth_landings->setText(il3.join(" "));

    QList<double> l4 = config.calib_weight_at_szgroup();
    QStringList il4;
    foreach (double i, l4)
        il4 << QString::number(i);

    ui->m_calib_weight_at_szgroup->setText(il4.join(" "));

    l1 = config.m_interesting_harbours;
    il.clear();
    foreach (int i, l1)
        il << QString::number(i);

    ui->m_int_harbours->setText(il.join(" "));
}

void ConfigDialog::on_readFromTree_clicked()
{
    QString x;
    const QList<int> & list = mModel->getInterestingHarbours();
    foreach(int i, list) {
        x += QString::number(i) + " ";
    }

    ui->m_int_harbours->setText(x);
}
