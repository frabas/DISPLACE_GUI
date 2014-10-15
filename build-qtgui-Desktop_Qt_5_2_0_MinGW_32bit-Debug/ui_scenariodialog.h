/********************************************************************************
** Form generated from reading UI file 'scenariodialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENARIODIALOG_H
#define UI_SCENARIODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ScenarioDialog
{
public:
    QGridLayout *gridLayout;
    QDoubleSpinBox *gridres;
    QSpinBox *aport;
    QSpinBox *nrowgraph;
    QSpinBox *nrowcoord;
    QSpinBox *agraph;
    QLabel *label;
    QLabel *label_6;
    QLabel *label_5;
    QLabel *label_7;
    QDialogButtonBox *buttonBox;
    QLabel *label_2;
    QLabel *label_8;
    QLineEdit *dyn_pop_sce;
    QLabel *label_3;
    QLineEdit *dyn_alloc_sce;
    QLabel *label_4;
    QSpacerItem *verticalSpacer;
    QLineEdit *biol_sce;

    void setupUi(QDialog *ScenarioDialog)
    {
        if (ScenarioDialog->objectName().isEmpty())
            ScenarioDialog->setObjectName(QStringLiteral("ScenarioDialog"));
        ScenarioDialog->resize(400, 325);
        gridLayout = new QGridLayout(ScenarioDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridres = new QDoubleSpinBox(ScenarioDialog);
        gridres->setObjectName(QStringLiteral("gridres"));
        gridres->setMaximum(10000);
        gridres->setSingleStep(0.5);

        gridLayout->addWidget(gridres, 7, 1, 1, 1);

        aport = new QSpinBox(ScenarioDialog);
        aport->setObjectName(QStringLiteral("aport"));
        aport->setMaximum(100000);

        gridLayout->addWidget(aport, 6, 1, 1, 1);

        nrowgraph = new QSpinBox(ScenarioDialog);
        nrowgraph->setObjectName(QStringLiteral("nrowgraph"));
        nrowgraph->setMaximum(1000000);

        gridLayout->addWidget(nrowgraph, 5, 1, 1, 1);

        nrowcoord = new QSpinBox(ScenarioDialog);
        nrowcoord->setObjectName(QStringLiteral("nrowcoord"));
        nrowcoord->setMaximum(1000000);

        gridLayout->addWidget(nrowcoord, 4, 1, 1, 1);

        agraph = new QSpinBox(ScenarioDialog);
        agraph->setObjectName(QStringLiteral("agraph"));
        agraph->setMaximum(100000);

        gridLayout->addWidget(agraph, 3, 1, 1, 1);

        label = new QLabel(ScenarioDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_6 = new QLabel(ScenarioDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        label_5 = new QLabel(ScenarioDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        label_7 = new QLabel(ScenarioDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 7, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ScenarioDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 9, 0, 1, 2);

        label_2 = new QLabel(ScenarioDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_8 = new QLabel(ScenarioDialog);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 4, 0, 1, 1);

        dyn_pop_sce = new QLineEdit(ScenarioDialog);
        dyn_pop_sce->setObjectName(QStringLiteral("dyn_pop_sce"));

        gridLayout->addWidget(dyn_pop_sce, 1, 1, 1, 1);

        label_3 = new QLabel(ScenarioDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        dyn_alloc_sce = new QLineEdit(ScenarioDialog);
        dyn_alloc_sce->setObjectName(QStringLiteral("dyn_alloc_sce"));

        gridLayout->addWidget(dyn_alloc_sce, 0, 1, 1, 1);

        label_4 = new QLabel(ScenarioDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 8, 1, 1, 1);

        biol_sce = new QLineEdit(ScenarioDialog);
        biol_sce->setObjectName(QStringLiteral("biol_sce"));

        gridLayout->addWidget(biol_sce, 2, 1, 1, 1);

        QWidget::setTabOrder(dyn_alloc_sce, dyn_pop_sce);
        QWidget::setTabOrder(dyn_pop_sce, biol_sce);
        QWidget::setTabOrder(biol_sce, agraph);
        QWidget::setTabOrder(agraph, nrowcoord);
        QWidget::setTabOrder(nrowcoord, nrowgraph);
        QWidget::setTabOrder(nrowgraph, aport);
        QWidget::setTabOrder(aport, gridres);

        retranslateUi(ScenarioDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ScenarioDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ScenarioDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ScenarioDialog);
    } // setupUi

    void retranslateUi(QDialog *ScenarioDialog)
    {
        ScenarioDialog->setWindowTitle(QApplication::translate("ScenarioDialog", "Scenario", 0));
        gridres->setSuffix(QApplication::translate("ScenarioDialog", " km", 0));
        label->setText(QApplication::translate("ScenarioDialog", "Dyn Alloc Sce:", 0));
        label_6->setText(QApplication::translate("ScenarioDialog", "A Port", 0));
        label_5->setText(QApplication::translate("ScenarioDialog", "N Row Graph", 0));
        label_7->setText(QApplication::translate("ScenarioDialog", "Grid Resolution:", 0));
        label_2->setText(QApplication::translate("ScenarioDialog", "Dyn Pop Sce:", 0));
        label_8->setText(QApplication::translate("ScenarioDialog", "N Row Coord", 0));
#ifndef QT_NO_TOOLTIP
        dyn_pop_sce->setToolTip(QApplication::translate("ScenarioDialog", "Enter values separated by spaces", 0));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("ScenarioDialog", "Bio Sce:", 0));
#ifndef QT_NO_TOOLTIP
        dyn_alloc_sce->setToolTip(QApplication::translate("ScenarioDialog", "Enter values separated by spaces", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("ScenarioDialog", "AGraph", 0));
    } // retranslateUi

};

namespace Ui {
    class ScenarioDialog: public Ui_ScenarioDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENARIODIALOG_H
