/********************************************************************************
** Form generated from reading UI file 'nodedetailswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NODEDETAILSWIDGET_H
#define UI_NODEDETAILSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NodeDetailsWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton;
    QTextEdit *text;

    void setupUi(QWidget *NodeDetailsWidget)
    {
        if (NodeDetailsWidget->objectName().isEmpty())
            NodeDetailsWidget->setObjectName(QStringLiteral("NodeDetailsWidget"));
        NodeDetailsWidget->resize(250, 226);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(NodeDetailsWidget->sizePolicy().hasHeightForWidth());
        NodeDetailsWidget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(NodeDetailsWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton = new QToolButton(NodeDetailsWidget);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setAutoFillBackground(false);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton->setIcon(icon);

        horizontalLayout->addWidget(toolButton);


        verticalLayout->addLayout(horizontalLayout);

        text = new QTextEdit(NodeDetailsWidget);
        text->setObjectName(QStringLiteral("text"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(text->sizePolicy().hasHeightForWidth());
        text->setSizePolicy(sizePolicy1);
        text->setFrameShape(QFrame::NoFrame);
        text->setReadOnly(true);

        verticalLayout->addWidget(text);


        retranslateUi(NodeDetailsWidget);
        QObject::connect(toolButton, SIGNAL(clicked()), NodeDetailsWidget, SLOT(close()));

        QMetaObject::connectSlotsByName(NodeDetailsWidget);
    } // setupUi

    void retranslateUi(QWidget *NodeDetailsWidget)
    {
        NodeDetailsWidget->setWindowTitle(QString());
        toolButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class NodeDetailsWidget: public Ui_NodeDetailsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NODEDETAILSWIDGET_H
