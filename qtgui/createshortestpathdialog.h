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
 * Copyright (c) 2012, 2013, 2014, 2015 Francois Bastardie <fba@aqua.dtu.dk>
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
#ifndef CREATESHORTESTPATHDIALOG_H
#define CREATESHORTESTPATHDIALOG_H

#include <QDialog>

namespace Ui {
class CreateShortestPathDialog;
}

class CreateShortestPathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateShortestPathDialog(QWidget *parent = 0);
    ~CreateShortestPathDialog();

    void setShortestPathFolder (QString);
    QString getShortestPathFolder() const;
    void setRelevantNodesFolder(QString);
    QString getRelevantNodesFolder() const;
    void setRelevantInterNodesFolder(QString);
    QString getRelevantInterNodesFolder() const;
    bool isAllNodesAreRelevantChecked() const;
    QString getOutputFolder() const;
    void setOutputFolder (const QString &folder);
    QString getGraphName() const;
    void setGraphName(const QString &folder);

    bool isBinaryFormat() const;
    bool isTextFormat() const;

    bool isAStarSelected() const;
    bool isDijkstraSelected() const;

private slots:
    void on_browseShortestFolder_clicked();
    void on_browseRelevantFolder_clicked();
    void on_browseRelevantInterFolder_clicked();
    void on_ok_clicked();
    void on_browseOutFolder_clicked();
    void on_graphName_textChanged(const QString &);
    void on_checkAllRelevantNodes_toggled(bool);

private:
    Ui::CreateShortestPathDialog *ui;
};

#endif // CREATESHORTESTPATHDIALOG_H
