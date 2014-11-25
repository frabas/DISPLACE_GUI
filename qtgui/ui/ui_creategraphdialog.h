/********************************************************************************
** Form generated from reading UI file 'creategraphdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEGRAPHDIALOG_H
#define UI_CREATEGRAPHDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_CreateGraphDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *long1;
    QLabel *label_3;
    QDoubleSpinBox *lat1;
    QLabel *label_4;
    QLabel *label_5;
    QDoubleSpinBox *long2;
    QLabel *label_6;
    QDoubleSpinBox *lat2;
    QLabel *label_7;
    QDoubleSpinBox *distance;
    QRadioButton *hex;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *cmdCancel;
    QPushButton *cmdOk;

    void setupUi(QDialog *CreateGraphDialog)
    {
        if (CreateGraphDialog->objectName().isEmpty())
            CreateGraphDialog->setObjectName(QStringLiteral("CreateGraphDialog"));
        CreateGraphDialog->resize(400, 300);
        gridLayout = new QGridLayout(CreateGraphDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(CreateGraphDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(CreateGraphDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        long1 = new QDoubleSpinBox(CreateGraphDialog);
        long1->setObjectName(QStringLiteral("long1"));

        gridLayout->addWidget(long1, 0, 2, 1, 1);

        label_3 = new QLabel(CreateGraphDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 0, 3, 1, 1);

        lat1 = new QDoubleSpinBox(CreateGraphDialog);
        lat1->setObjectName(QStringLiteral("lat1"));

        gridLayout->addWidget(lat1, 0, 4, 1, 1);

        label_4 = new QLabel(CreateGraphDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        label_5 = new QLabel(CreateGraphDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 1, 1, 1);

        long2 = new QDoubleSpinBox(CreateGraphDialog);
        long2->setObjectName(QStringLiteral("long2"));

        gridLayout->addWidget(long2, 1, 2, 1, 1);

        label_6 = new QLabel(CreateGraphDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 1, 3, 1, 1);

        lat2 = new QDoubleSpinBox(CreateGraphDialog);
        lat2->setObjectName(QStringLiteral("lat2"));

        gridLayout->addWidget(lat2, 1, 4, 1, 1);

        label_7 = new QLabel(CreateGraphDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 2, 0, 1, 1);

        distance = new QDoubleSpinBox(CreateGraphDialog);
        distance->setObjectName(QStringLiteral("distance"));

        gridLayout->addWidget(distance, 2, 1, 1, 2);

        hex = new QRadioButton(CreateGraphDialog);
        hex->setObjectName(QStringLiteral("hex"));

        gridLayout->addWidget(hex, 2, 3, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 3, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cmdCancel = new QPushButton(CreateGraphDialog);
        cmdCancel->setObjectName(QStringLiteral("cmdCancel"));

        horizontalLayout->addWidget(cmdCancel);

        cmdOk = new QPushButton(CreateGraphDialog);
        cmdOk->setObjectName(QStringLiteral("cmdOk"));

        horizontalLayout->addWidget(cmdOk);


        gridLayout->addLayout(horizontalLayout, 4, 2, 1, 3);


        retranslateUi(CreateGraphDialog);
        QObject::connect(cmdOk, SIGNAL(clicked()), CreateGraphDialog, SLOT(accept()));
        QObject::connect(cmdCancel, SIGNAL(clicked()), CreateGraphDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CreateGraphDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateGraphDialog)
    {
        CreateGraphDialog->setWindowTitle(QApplication::translate("CreateGraphDialog", "Dialog", 0));
        label->setText(QApplication::translate("CreateGraphDialog", "Corner 1", 0));
        label_2->setText(QApplication::translate("CreateGraphDialog", "Long:", 0));
        label_3->setText(QApplication::translate("CreateGraphDialog", "Lat:", 0));
        label_4->setText(QApplication::translate("CreateGraphDialog", "Corner 2", 0));
        label_5->setText(QApplication::translate("CreateGraphDialog", "Long:", 0));
        label_6->setText(QApplication::translate("CreateGraphDialog", "Lat:", 0));
        label_7->setText(QApplication::translate("CreateGraphDialog", "Distance", 0));
        distance->setSuffix(QApplication::translate("CreateGraphDialog", " km", 0));
        hex->setText(QApplication::translate("CreateGraphDialog", "Hex", 0));
        cmdCancel->setText(QApplication::translate("CreateGraphDialog", "Cancel", 0));
        cmdOk->setText(QApplication::translate("CreateGraphDialog", "Ok", 0));
    } // retranslateUi

};

namespace Ui {
    class CreateGraphDialog: public Ui_CreateGraphDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEGRAPHDIALOG_H
