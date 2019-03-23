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
#ifndef PATHPENALTYDIALOG_H
#define PATHPENALTYDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QCheckBox;
QT_END_NAMESPACE

namespace Ui {
class PathPenaltyDialog;
}

class PathPenaltyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPenaltyDialog(QWidget *parent = 0);
    ~PathPenaltyDialog();

    void showShapefileOptions(bool show);
    void setShapefileList(QStringList files);
    void setMetierNumber(int num);
    QStringList selectedShapefile() const;
    double nbOfDaysClosedPerMonth() const;
    bool isPenaltyQ1();
    bool isPenaltyQ2();
    bool isPenaltyQ3();
    bool isPenaltyQ4();
    double weight() const;

    std::vector<bool> getCheckedMonths() const;
    std::vector<int> getBannedMetiers() const;
    std::vector<int> getVesSizeSelection() const;

private slots:
    void on_ok_clicked();
    void cbToggled(bool);
    void metierEnableToggled(bool);

    void on_enableSizes_toggled(bool checked);

private:
    Ui::PathPenaltyDialog *ui;

    QGridLayout *mShapefileGrid, *mMetierGrid;
    std::vector<QCheckBox *> mShapefileCheckboxes;
    std::vector<QCheckBox *> mMetierCheckboxes;
    std::vector<QCheckBox *> mMonthsCheckboxes;

    int clickCount = 0;
};

#endif // PATHPENALTYDIALOG_H
