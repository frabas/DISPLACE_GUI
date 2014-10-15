/********************************************************************************
** Form generated from reading UI file 'simulationsetupdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMULATIONSETUPDIALOG_H
#define UI_SIMULATIONSETUPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_SimulationSetupDialog
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QSpinBox *simSteps;
    QLabel *label;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *SimulationSetupDialog)
    {
        if (SimulationSetupDialog->objectName().isEmpty())
            SimulationSetupDialog->setObjectName(QStringLiteral("SimulationSetupDialog"));
        SimulationSetupDialog->resize(400, 300);
        gridLayout = new QGridLayout(SimulationSetupDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buttonBox = new QDialogButtonBox(SimulationSetupDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 2);

        simSteps = new QSpinBox(SimulationSetupDialog);
        simSteps->setObjectName(QStringLiteral("simSteps"));
        simSteps->setMinimum(1);
        simSteps->setMaximum(30000);
        simSteps->setValue(8000);

        gridLayout->addWidget(simSteps, 0, 1, 1, 1);

        label = new QLabel(SimulationSetupDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);


        retranslateUi(SimulationSetupDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SimulationSetupDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SimulationSetupDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SimulationSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *SimulationSetupDialog)
    {
        SimulationSetupDialog->setWindowTitle(QApplication::translate("SimulationSetupDialog", "Simulation Setup", 0));
        simSteps->setSuffix(QApplication::translate("SimulationSetupDialog", " steps", 0));
        label->setText(QApplication::translate("SimulationSetupDialog", "Simulation Length", 0));
    } // retranslateUi

};

namespace Ui {
    class SimulationSetupDialog: public Ui_SimulationSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMULATIONSETUPDIALOG_H
