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

#include "nodemappingsdialog.h"
#include "ui_nodemappingsdialog.h"

#include <dtree/dtnode.h>

#include <QLayout>
#include <QLabel>

NodeMappingsDialog::NodeMappingsDialog(std::shared_ptr<dtree::Node> node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NodeMappingsDialog),
    mNode(node)
{
    ui->setupUi(this);

    mGroupLayout = new QGridLayout();
    ui->mappingsGroup->setLayout(mGroupLayout);

    setupMappings();
}

NodeMappingsDialog::~NodeMappingsDialog()
{
    delete ui;
}

int NodeMappingsDialog::getMappingForIndex(int idx)
{
    if (idx >= 0 && idx < mCombos.size()) {
        return mCombos.at(idx)->currentIndex();
    }

    return -1;
}

void NodeMappingsDialog::setupMappings()
{
    int n = mNode->getChildrenCount();

    int mx = 1;
    for (int i = 0; i < n; ++i) {
        QLabel *label = new QLabel();
        label->setText(tr("Output %1").arg(i));
        mGroupLayout->addWidget(label, i, 0);

        QComboBox *box = new QComboBox();
        mGroupLayout->addWidget(box, i, 1);

        mCombos.append(box);

        mx = std::max(mx, mNode->getMapping(i));
    }

    ui->groupNumber->setValue(mx+1);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < mx+1; ++j) {
            mCombos[i]->addItem(QString(tr("%1")).arg(j));
        }

        mCombos[i]->setCurrentIndex(mNode->getMapping(i));
    }
}
