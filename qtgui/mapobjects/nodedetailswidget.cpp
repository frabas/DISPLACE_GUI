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

#include "nodedetailswidget.h"
#include "ui_nodedetailswidget.h"

NodeDetailsWidget::NodeDetailsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeDetailsWidget)
{
    ui->setupUi(this);
    ui->tool1->setVisible(false);

    connect (ui->tool1, SIGNAL(clicked()), this, SIGNAL(toolButtonClicked()), Qt::QueuedConnection);
}

NodeDetailsWidget::~NodeDetailsWidget()
{
    delete ui;
}

void NodeDetailsWidget::setText(QString text)
{   
    if (ui && ui->text) {
        ui->text->setText(text);
    }
    else {
        qWarning() << "NodeDetailsWidget::setText called but UI element missing";
    }
}

void NodeDetailsWidget::showTool(bool display)
{
    ui->tool1->setVisible(display);
}
