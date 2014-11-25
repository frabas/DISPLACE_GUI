/********************************************************************************
** Form generated from reading UI file 'simulationsetupdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMULATIONSETUPDIALOG_H
#define UI_SIMULATIONSETUPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_SimulationSetupDialog
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLineEdit *simName;
    QLabel *label_3;
    QSpinBox *simSteps;
    QDialogButtonBox *buttonBox;
    QLabel *label_2;
    QLineEdit *outName;
    QLabel *label;
    QCheckBox *optMoveVessels;

    void setupUi(QDialog *SimulationSetupDialog)
    {
        if (SimulationSetupDialog->objectName().isEmpty())
            SimulationSetupDialog->setObjectName(QStringLiteral("SimulationSetupDialog"));
        SimulationSetupDialog->resize(400, 300);
        gridLayout = new QGridLayout(SimulationSetupDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 0, 1, 1);

        simName = new QLineEdit(SimulationSetupDialog);
        simName->setObjectName(QStringLiteral("simName"));

        gridLayout->addWidget(simName, 2, 1, 1, 1);

        label_3 = new QLabel(SimulationSetupDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        simSteps = new QSpinBox(SimulationSetupDialog);
        simSteps->setObjectName(QStringLiteral("simSteps"));
        simSteps->setMinimum(1);
        simSteps->setMaximum(100000);
        simSteps->setValue(8000);

        gridLayout->addWidget(simSteps, 0, 1, 1, 1);

        buttonBox = new QDialogButtonBox(SimulationSetupDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 5, 0, 1, 2);

        label_2 = new QLabel(SimulationSetupDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        outName = new QLineEdit(SimulationSetupDialog);
        outName->setObjectName(QStringLiteral("outName"));

        gridLayout->addWidget(outName, 1, 1, 1, 1);

        label = new QLabel(SimulationSetupDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        optMoveVessels = new QCheckBox(SimulationSetupDialog);
        optMoveVessels->setObjectName(QStringLiteral("optMoveVessels"));

        gridLayout->addWidget(optMoveVessels, 3, 0, 1, 2);


        retranslateUi(SimulationSetupDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SimulationSetupDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SimulationSetupDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SimulationSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *SimulationSetupDialog)
    {
        SimulationSetupDialog->setWindowTitle(QApplication::translate("SimulationSetupDialog", "Simulation Setup", 0));
        label_3->setText(QApplication::translate("SimulationSetupDialog", "Simulation Name", 0));
        simSteps->setSuffix(QApplication::translate("SimulationSetupDialog", " steps", 0));
        label_2->setText(QApplication::translate("SimulationSetupDialog", "Output folder name", 0));
        label->setText(QApplication::translate("SimulationSetupDialog", "Simulation Length", 0));
        optMoveVessels->setText(QApplication::translate("SimulationSetupDialog", "Record Vessels position", 0));
    } // retranslateUi

};

namespace Ui {
    class SimulationSetupDialog: public Ui_SimulationSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMULATIONSETUPDIALOG_H
