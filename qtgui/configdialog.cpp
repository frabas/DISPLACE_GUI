// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

    QList<types::NodeId> ih;
    l = ui->m_int_harbours->text().split(" ", QString::SkipEmptyParts);
    foreach(QString s, l) {
        ih.push_back(types::NodeId(s.toInt(&ok)));
        if (!ok)
            return false;
    }

    config.setNbpops(ui->nbpops->value());
    config.setNbbenthospops(ui->nbbenthospops->value());
    config.setSzGroups(ui->szGroups->value());
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
    ui->nbbenthospops->setValue(config.getNbbenthospops());
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

    auto hl = config.m_interesting_harbours;
    il.clear();
    foreach (auto i, hl)
        il << QString::number(i.toIndex());

    ui->m_int_harbours->setText(il.join(" "));
}

void ConfigDialog::on_readFromTree_clicked()
{
    QString x;
    const auto & list = mModel->getInterestingHarbours();
    foreach(auto i, list) {
        x += QString::number(i.toIndex()) + " ";
    }

    ui->m_int_harbours->setText(x);
}
