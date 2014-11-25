/********************************************************************************
** Form generated from reading UI file 'configdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label_6;
    QLabel *label;
    QDialogButtonBox *buttonBox;
    QSpacerItem *verticalSpacer;
    QLineEdit *m_int_harbours;
    QLabel *label_4;
    QLabel *label_7;
    QLabel *label_3;
    QLabel *label_5;
    QToolButton *readFromTree;
    QLineEdit *m_calib_cpue_multiplier;
    QLineEdit *m_calib_weight_at_szgroup;
    QLineEdit *m_calib_oth_landings;
    QLineEdit *m_implicit_pops;
    QSpinBox *szGroups;
    QSpinBox *nbpops;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QStringLiteral("ConfigDialog"));
        ConfigDialog->resize(400, 300);
        gridLayout = new QGridLayout(ConfigDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(ConfigDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_6 = new QLabel(ConfigDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        label = new QLabel(ConfigDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ConfigDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 9, 0, 1, 3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 8, 0, 1, 1);

        m_int_harbours = new QLineEdit(ConfigDialog);
        m_int_harbours->setObjectName(QStringLiteral("m_int_harbours"));

        gridLayout->addWidget(m_int_harbours, 7, 2, 1, 1);

        label_4 = new QLabel(ConfigDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        label_7 = new QLabel(ConfigDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 7, 0, 1, 1);

        label_3 = new QLabel(ConfigDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label_5 = new QLabel(ConfigDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        readFromTree = new QToolButton(ConfigDialog);
        readFromTree->setObjectName(QStringLiteral("readFromTree"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/database.png"), QSize(), QIcon::Normal, QIcon::Off);
        readFromTree->setIcon(icon);

        gridLayout->addWidget(readFromTree, 7, 3, 1, 1);

        m_calib_cpue_multiplier = new QLineEdit(ConfigDialog);
        m_calib_cpue_multiplier->setObjectName(QStringLiteral("m_calib_cpue_multiplier"));

        gridLayout->addWidget(m_calib_cpue_multiplier, 6, 2, 1, 2);

        m_calib_weight_at_szgroup = new QLineEdit(ConfigDialog);
        m_calib_weight_at_szgroup->setObjectName(QStringLiteral("m_calib_weight_at_szgroup"));

        gridLayout->addWidget(m_calib_weight_at_szgroup, 5, 2, 1, 2);

        m_calib_oth_landings = new QLineEdit(ConfigDialog);
        m_calib_oth_landings->setObjectName(QStringLiteral("m_calib_oth_landings"));

        gridLayout->addWidget(m_calib_oth_landings, 3, 2, 1, 2);

        m_implicit_pops = new QLineEdit(ConfigDialog);
        m_implicit_pops->setObjectName(QStringLiteral("m_implicit_pops"));

        gridLayout->addWidget(m_implicit_pops, 2, 2, 1, 2);

        szGroups = new QSpinBox(ConfigDialog);
        szGroups->setObjectName(QStringLiteral("szGroups"));
        szGroups->setReadOnly(true);
        szGroups->setMinimum(1);
        szGroups->setValue(14);

        gridLayout->addWidget(szGroups, 1, 2, 1, 2);

        nbpops = new QSpinBox(ConfigDialog);
        nbpops->setObjectName(QStringLiteral("nbpops"));

        gridLayout->addWidget(nbpops, 0, 2, 1, 2);


        retranslateUi(ConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConfigDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Dialog", 0));
        label_2->setText(QApplication::translate("ConfigDialog", "Number of Size Groups", 0));
        label_6->setText(QApplication::translate("ConfigDialog", "calib the cpue multiplier per stock", 0));
        label->setText(QApplication::translate("ConfigDialog", "Number of populations", 0));
        label_4->setText(QApplication::translate("ConfigDialog", "calib the other landings per stock", 0));
        label_7->setText(QApplication::translate("ConfigDialog", "Interesting Harbours", 0));
        label_3->setText(QApplication::translate("ConfigDialog", "Implicit Stocks", 0));
        label_5->setText(QApplication::translate("ConfigDialog", "calib weight-at-szgroup per stock", 0));
        readFromTree->setText(QApplication::translate("ConfigDialog", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H
