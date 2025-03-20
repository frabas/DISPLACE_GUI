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

#ifndef MERGEDATADIALOG_H
#define MERGEDATADIALOG_H

#include <QDialog>

namespace Ui {
class MergeDataDialog;
}

/** \brief Implements the functionalities for mergedatadialog.ui
 *
 * This UI can be extended by derived classes by "plugging" additional components through the protected function insertPluggableComponent().
 * See for example \see MergePopulationDataDialog .
 * */
class MergeDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeDataDialog(QWidget *parent = 0);
    ~MergeDataDialog();

    QString getInputFile() const;
    QString getOutputFile() const;
    void setInputFile (QString setInputFile);
    void setOutputFile (QString setOutputFile);
    void setDefaultOutputToInput(bool yes) {
        mDefaultOutToIn = yes;
    }
    void setOutputRequiresTemplate(int numtemplates) {
        mOutRequiresTemplate = numtemplates;
    }

    double getDistance() const;

    QChar separator() const;

private slots:
    void on_ok_clicked();
    void on_browseIn_clicked();
    void on_browseOut_clicked();

protected:
    void insertPluggableComponent (QWidget *widget);

    virtual void inputFileSelected() {}

private:
    Ui::MergeDataDialog *ui;

    bool mDefaultOutToIn = true;
    int mOutRequiresTemplate = 0;

    static double lastDistance;
};

#endif // MERGEDATADIALOG_H
