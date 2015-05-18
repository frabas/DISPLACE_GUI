#include "nodemappingsdialog.h"
#include "ui_nodemappingsdialog.h"

#include <dtree/dtnode.h>

#include <QLayout>
#include <QLabel>

NodeMappingsDialog::NodeMappingsDialog(boost::shared_ptr<dtree::Node> node, QWidget *parent) :
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

    int mx = 2;
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
