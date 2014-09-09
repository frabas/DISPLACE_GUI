/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>
#include "mygraphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTreeWidget *treeWidget;
    MyGraphicsView *view;
    QScrollBar *verticalScrollBar;
    QPushButton *startButton;
    QPushButton *quitButton;
    QSpinBox *spinBox;
    QLabel *label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(700, 700);
        MainWindow->setMinimumSize(QSize(700, 700));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setMinimumSize(QSize(500, 500));
        treeWidget = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setGeometry(QRect(10, 10, 101, 401));
        view = new MyGraphicsView(centralWidget);
        view->setObjectName(QStringLiteral("view"));
        view->setGeometry(QRect(120, 10, 441, 401));
        view->setMinimumSize(QSize(400, 100));
        view->setAutoFillBackground(true);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setTransformationAnchor(QGraphicsView::NoAnchor);
        view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        verticalScrollBar = new QScrollBar(centralWidget);
        verticalScrollBar->setObjectName(QStringLiteral("verticalScrollBar"));
        verticalScrollBar->setGeometry(QRect(90, 30, 20, 361));
        verticalScrollBar->setOrientation(Qt::Vertical);
        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(360, 420, 91, 21));
        quitButton = new QPushButton(centralWidget);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setGeometry(QRect(460, 420, 101, 21));
        spinBox = new QSpinBox(centralWidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setGeometry(QRect(640, 40, 41, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(570, 40, 61, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 700, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(quitButton, SIGNAL(clicked()), MainWindow, SLOT(close()));
        QObject::connect(startButton, SIGNAL(clicked()), MainWindow, SLOT(update()));
        QObject::connect(spinBox, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "DISPLACE ", 0));
        startButton->setText(QApplication::translate("MainWindow", "START", 0));
        quitButton->setText(QApplication::translate("MainWindow", "QUIT", 0));
        label->setText(QApplication::translate("MainWindow", "nb time step", 0));
        mainToolBar->setWindowTitle(QApplication::translate("MainWindow", "DISPLACE model", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
