// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef EDITPALETTEDIALOG_H
#define EDITPALETTEDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
    class EditPaletteDialog;
}

class Palette;
class PaletteModel;
class SpecialPaletteModel;

class EditPaletteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPaletteDialog(QWidget *parent = 0);
    ~EditPaletteDialog();

    void showSpecials(bool visible = true);

    void linkPalette (Palette *palette);

signals:
    void paletteChanged();

private slots:
    void on_palette_doubleClicked(const QModelIndex &index);
    void on_specialPalette_doubleClicked(const QModelIndex &index);
    void on_btLoad_clicked();
    void on_btSave_clicked();
    void on_name_textChanged(const QString &arg1);
    void on_add_clicked();
    void on_remove_clicked();

    void on_pushButton_clicked();

protected:
    void updateControlValues();

private:
    Ui::EditPaletteDialog *ui;
    Palette *mPalette;
    PaletteModel *mModel;
    SpecialPaletteModel *mSpecialModel;

    static QString mFileFilter;
};

#endif // EDITPALETTEDIALOG_H
