// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef CREATEGRAPHDIALOG_H
#define CREATEGRAPHDIALOG_H

#include <QDialog>
#include <graphbuilder_shp.h>

namespace Ui {
class CreateGraphDialog;
}

class CreateGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGraphDialog(QWidget *parent = 0);
    ~CreateGraphDialog();

    double defaultStep() const;
    double step1() const;
    double step2() const;
    double minLon() const;
    double minLat() const;
    double maxLon() const;
    double maxLat() const;
    GraphBuilder::Type getType() const;

    void setShapefileList(QStringList list);
    QString getIncludingSelectedShapefile1() const ;
    QString getIncludingSelectedShapefile2() const ;
    QString getExcludingSelectedShapefile() const ;

    bool isOutsideEnabled() const;
    bool isRemoveLongEdgesEnabled() const;
    bool isRemoveEdgesInExclusionZoneEnabled() const;
    double removeLongEdgesLimit() const;
    bool isMaxLinksEnabled() const;
    bool isMinLinksEnabled() const;
    int getMaxLinks() const;
    int getMinLinks() const;

    bool isCreateEnabled() const;
    bool isLoadEnabled() const;
    QString loadGraphPath() const;

public slots:
    void done(int) override;

    void onLoadBrowseButtonClicked();
    void onLoadRadioSelected(bool checked);
    void onCreateRadioSelected(bool checked);

private:
    Ui::CreateGraphDialog *ui;

    void updateView() const;
};

#endif // CREATEGRAPHDIALOG_H
