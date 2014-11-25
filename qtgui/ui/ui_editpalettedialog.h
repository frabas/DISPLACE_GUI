/********************************************************************************
** Form generated from reading UI file 'editpalettedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITPALETTEDIALOG_H
#define UI_EDITPALETTEDIALOG_H

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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EditPaletteDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLineEdit *name;
    QGridLayout *gridLayout;
    QLabel *label;
    QDoubleSpinBox *minVal;
    QLabel *label_3;
    QSpinBox *nrCol;
    QDoubleSpinBox *maxVal;
    QDoubleSpinBox *step;
    QLabel *label_2;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QTableView *palette;
    QTableView *specialPalette;
    QVBoxLayout *verticalLayout;
    QToolButton *btLoad;
    QToolButton *btSave;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;

    void setupUi(QDialog *EditPaletteDialog)
    {
        if (EditPaletteDialog->objectName().isEmpty())
            EditPaletteDialog->setObjectName(QStringLiteral("EditPaletteDialog"));
        EditPaletteDialog->resize(441, 533);
        verticalLayout_3 = new QVBoxLayout(EditPaletteDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(EditPaletteDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);

        name = new QLineEdit(EditPaletteDialog);
        name->setObjectName(QStringLiteral("name"));

        horizontalLayout_3->addWidget(name);


        verticalLayout_3->addLayout(horizontalLayout_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(EditPaletteDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        minVal = new QDoubleSpinBox(EditPaletteDialog);
        minVal->setObjectName(QStringLiteral("minVal"));

        gridLayout->addWidget(minVal, 1, 1, 1, 1);

        label_3 = new QLabel(EditPaletteDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        nrCol = new QSpinBox(EditPaletteDialog);
        nrCol->setObjectName(QStringLiteral("nrCol"));
        nrCol->setMinimum(1);

        gridLayout->addWidget(nrCol, 2, 1, 1, 1);

        maxVal = new QDoubleSpinBox(EditPaletteDialog);
        maxVal->setObjectName(QStringLiteral("maxVal"));
        maxVal->setMaximum(100000);

        gridLayout->addWidget(maxVal, 1, 3, 1, 1);

        step = new QDoubleSpinBox(EditPaletteDialog);
        step->setObjectName(QStringLiteral("step"));
        step->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        step->setDecimals(1);
        step->setMinimum(0.5);
        step->setSingleStep(0.5);

        gridLayout->addWidget(step, 2, 3, 1, 1);

        label_2 = new QLabel(EditPaletteDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 2, 1, 1);

        label_4 = new QLabel(EditPaletteDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 2, 2, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        palette = new QTableView(EditPaletteDialog);
        palette->setObjectName(QStringLiteral("palette"));
        palette->horizontalHeader()->setVisible(false);
        palette->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(palette);

        specialPalette = new QTableView(EditPaletteDialog);
        specialPalette->setObjectName(QStringLiteral("specialPalette"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(specialPalette->sizePolicy().hasHeightForWidth());
        specialPalette->setSizePolicy(sizePolicy);
        specialPalette->setMinimumSize(QSize(0, 50));
        specialPalette->setMaximumSize(QSize(16777215, 50));

        verticalLayout_2->addWidget(specialPalette);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        btLoad = new QToolButton(EditPaletteDialog);
        btLoad->setObjectName(QStringLiteral("btLoad"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/load.png"), QSize(), QIcon::Normal, QIcon::Off);
        btLoad->setIcon(icon);

        verticalLayout->addWidget(btLoad);

        btSave = new QToolButton(EditPaletteDialog);
        btSave->setObjectName(QStringLiteral("btSave"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/save"), QSize(), QIcon::Normal, QIcon::Off);
        btSave->setIcon(icon1);

        verticalLayout->addWidget(btSave);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_2->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_2 = new QPushButton(EditPaletteDialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(EditPaletteDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);


        verticalLayout_3->addLayout(horizontalLayout);

        QWidget::setTabOrder(name, minVal);
        QWidget::setTabOrder(minVal, maxVal);
        QWidget::setTabOrder(maxVal, nrCol);
        QWidget::setTabOrder(nrCol, step);
        QWidget::setTabOrder(step, palette);
        QWidget::setTabOrder(palette, specialPalette);
        QWidget::setTabOrder(specialPalette, btLoad);
        QWidget::setTabOrder(btLoad, btSave);
        QWidget::setTabOrder(btSave, pushButton_2);
        QWidget::setTabOrder(pushButton_2, pushButton);

        retranslateUi(EditPaletteDialog);
        QObject::connect(pushButton_2, SIGNAL(clicked()), EditPaletteDialog, SLOT(reject()));
        QObject::connect(pushButton, SIGNAL(clicked()), EditPaletteDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(EditPaletteDialog);
    } // setupUi

    void retranslateUi(QDialog *EditPaletteDialog)
    {
        EditPaletteDialog->setWindowTitle(QApplication::translate("EditPaletteDialog", "Palette editor", 0));
        label_5->setText(QApplication::translate("EditPaletteDialog", "Palette name/description:", 0));
        label->setText(QApplication::translate("EditPaletteDialog", "Min Value:", 0));
        label_3->setText(QApplication::translate("EditPaletteDialog", "Colours Nr.:", 0));
        label_2->setText(QApplication::translate("EditPaletteDialog", "Max Value:", 0));
        label_4->setText(QApplication::translate("EditPaletteDialog", "Step:", 0));
        btLoad->setText(QApplication::translate("EditPaletteDialog", "Load", 0));
        btSave->setText(QApplication::translate("EditPaletteDialog", "Save", 0));
        pushButton_2->setText(QApplication::translate("EditPaletteDialog", "Cancel", 0));
        pushButton->setText(QApplication::translate("EditPaletteDialog", "Apply", 0));
    } // retranslateUi

};

namespace Ui {
    class EditPaletteDialog: public Ui_EditPaletteDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITPALETTEDIALOG_H
