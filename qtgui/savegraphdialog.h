// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2107 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef SAVEGRAPHDIALOG_H
#define SAVEGRAPHDIALOG_H

#include <QDialog>

namespace Ui {
class SaveGraphDialog;
}

class SaveGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveGraphDialog(QWidget *parent = 0);
    ~SaveGraphDialog();

    void setName(QString name);
    void setOutputFolder(QString path);
    QString getGraphFilename() const;
    QString getCoordsFilename() const;
    QString getLandscapeFilename() const;
    QString getWindFilename() const;
    QString getSSTFilename() const;
    QString getSalinityFilename() const;
    QString getBenthosFilename() const;
    QString getBenthosNbFilename() const;
    QString getAreacodesFilename() const;
    QString getClosedPolygonFilename() const;
    QString getClosedPolygonFilenameMonthly() const;
    QString getClosedPolygonFilenameVesSize() const;
    bool isClosedPolygonExportChecked() const;

private slots:
    void on_optCustomize_toggled(bool checked);
    void on_graphName_textChanged(const QString &arg1);
    void on_cmdBrowse_clicked();

private:
    Ui::SaveGraphDialog *ui;
};

#endif // SAVEGRAPHDIALOG_H
