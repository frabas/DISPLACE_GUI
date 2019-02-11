// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef SCENARIODIALOG_H
#define SCENARIODIALOG_H

#include <QDialog>
#include <scenario.h>

namespace Ui {
class ScenarioDialog;
}

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
QT_END_NAMESPACE

class ScenarioDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScenarioDialog(const Scenario & scenario, QWidget *parent = 0);
    ~ScenarioDialog();

    const Scenario &getScenario() const { return mScen; }

    QString getScenarioPath() const;
    void setScenarioPath(const QString &value);
    bool isRenamed() const { return mRenamed; }

    void setForceRename() { mForceRenamed = true; }
private slots:
    void on_ScenarioDialog_accepted();
    void on_rename_clicked();
    void on_apply_clicked();
    void on_graphBrowse_clicked();
    void on_dt_enable_go_fishing_toggled(bool checked);
    void on_dt_enable_choose_ground_toggled(bool checked);
    void on_dt_enable_start_fishing_toggled(bool checked);
    void on_dt_enable_change_ground_toggled(bool checked);
    void on_dt_enable_stop_fishing_toggled(bool checked);
    void on_dt_enable_change_port_toggled(bool checked);
    void on_dt_browse_go_fishing_clicked();
    void on_dt_browse_choose_ground_clicked();
    void on_dt_browse_start_fishing_clicked();
    void on_dt_browse_change_ground_clicked();
    void on_dt_browse_stop_fishing_clicked();
    void on_dt_browse_change_port_clicked();

private:
    void setDynPop(const QStringList &text);
    void setDynAlloc(const QStringList &options);

private:
    Ui::ScenarioDialog *ui;

    Scenario mScen;
    QList<QCheckBox *> optPop;
    QList<QCheckBox *> optAll;
    QString mScenarioPath;
    bool mRenamed, mForceRenamed;
    void browse(QString title, QLineEdit *ed);
};

#endif // SCENARIODIALOG_H
