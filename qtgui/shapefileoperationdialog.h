// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef SHAPEFILEOPERATIONDIALOG_H
#define SHAPEFILEOPERATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ShapefileOperationDialog;
}

class ShapefileOperationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShapefileOperationDialog(QWidget *parent = 0);
    ~ShapefileOperationDialog();

    void setShapefileList(QStringList list);
    QString selectedShapefile() const;

private slots:
    void on_radioSelectShapefile_toggled(bool checked);
    void on_radioImportShapefile_toggled(bool checked);
    void on_cmdBrowse_clicked();

private:
    Ui::ShapefileOperationDialog *ui;
};

#endif // SHAPEFILEOPERATIONDIALOG_H
