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
#ifndef NODEDETAILSWIDGET_H
#define NODEDETAILSWIDGET_H

#include <QWidget>

namespace Ui {
class NodeDetailsWidget;
}

class NodeDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeDetailsWidget(QWidget *parent = 0);
    ~NodeDetailsWidget();

    void setText(QString text);
    void showTool (bool display);

signals:
    void toolButtonClicked();

private:
    Ui::NodeDetailsWidget *ui;
};

#endif // NODEDETAILSWIDGET_H
