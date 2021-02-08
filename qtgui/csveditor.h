// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef CSVEDITOR_H
#define CSVEDITOR_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

#include <csv/csvtablemodel.h>
#include <waitdialog.h>
#include <csv/csvimporter.h>

#include <memory>

namespace Ui {
class CsvEditor;
}

class CsvEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit CsvEditor(QWidget *parent = 0);
    ~CsvEditor();

private slots:
    void onLoadFinished();
    void separatorChanged(QAction *);

    void on_action_Open_triggered();
    void on_actionFirst_line_as_Headers_toggled(bool);
    void on_action_Save_triggered();
    void on_action_Insert_row_after_triggered();
    void on_action_Insert_row_at_current_position_triggered();
    void on_action_Remove_current_row_triggered();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::CsvEditor *ui;
    CsvTableModel *mModel;
    std::shared_ptr<QList<QStringList> > mData;
    QString mFilename;
    QChar mSeparator;

    QFuture<QList<QStringList>> mWorkLoading;
    QFutureWatcher<QList<QStringList>> mWorkLoadingWatcher;
    WaitDialog *mWaitDialog;
    CsvImporter mImporter;

    void load (QString file);
    void updateCheckState(bool state);
};

#endif // CSVEDITOR_H
