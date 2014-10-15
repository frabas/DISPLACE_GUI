/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "wrapperwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Load;
    QAction *actionScenario;
    QAction *actionSave;
    QAction *actionQuit;
    QAction *actionLoad_results;
    QAction *actionImport_results;
    QAction *action_Link_database;
    QAction *actionConfiguration;
    QAction *actionGraph;
    QAction *actionAdd_Polygon;
    QAction *actionImport_Shapefile;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    WrapperWidget *mapWidget;
    QMenuBar *menubar;
    QMenu *menu_Model;
    QMenu *menu_File;
    QMenu *menu_Editor;
    QStatusBar *statusbar;
    QDockWidget *objectMapView;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_4;
    QComboBox *modelSelector;
    QTreeView *treeView;
    QDockWidget *consoleView;
    QWidget *dockWidgetContents_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *saveConsoleButton;
    QSpacerItem *horizontalSpacer;
    QPlainTextEdit *console;
    QDockWidget *statsView;
    QWidget *dockWidgetContents_3;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_6;
    QTabWidget *statsTab;
    QWidget *tabStatsHarbours;
    QVBoxLayout *verticalLayout_7;
    QTableView *statsTableHarbours;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents_4;
    QVBoxLayout *verticalLayout_5;
    QPushButton *cmdStart;
    QPushButton *cmdPause;
    QPushButton *cmdStop;
    QPushButton *cmdSetup;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QLabel *info_simstep;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *play_last;
    QToolButton *play_first;
    QToolButton *play_ffwd;
    QToolButton *play_fwd;
    QToolButton *play_fbk;
    QToolButton *play_bk;
    QSpinBox *play_step;
    QToolButton *play_auto;
    QSpacerItem *horizontalSpacer_3;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(962, 692);
        action_Load = new QAction(MainWindow);
        action_Load->setObjectName(QStringLiteral("action_Load"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/loadsim.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Load->setIcon(icon);
        actionScenario = new QAction(MainWindow);
        actionScenario->setObjectName(QStringLiteral("actionScenario"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon1);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/quit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon2);
        actionLoad_results = new QAction(MainWindow);
        actionLoad_results->setObjectName(QStringLiteral("actionLoad_results"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/load.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoad_results->setIcon(icon3);
        actionImport_results = new QAction(MainWindow);
        actionImport_results->setObjectName(QStringLiteral("actionImport_results"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/database.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImport_results->setIcon(icon4);
        action_Link_database = new QAction(MainWindow);
        action_Link_database->setObjectName(QStringLiteral("action_Link_database"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/link.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Link_database->setIcon(icon5);
        actionConfiguration = new QAction(MainWindow);
        actionConfiguration->setObjectName(QStringLiteral("actionConfiguration"));
        actionGraph = new QAction(MainWindow);
        actionGraph->setObjectName(QStringLiteral("actionGraph"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/graph.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGraph->setIcon(icon6);
        actionAdd_Polygon = new QAction(MainWindow);
        actionAdd_Polygon->setObjectName(QStringLiteral("actionAdd_Polygon"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icons/geometry.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAdd_Polygon->setIcon(icon7);
        actionImport_Shapefile = new QAction(MainWindow);
        actionImport_Shapefile->setObjectName(QStringLiteral("actionImport_Shapefile"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/icons/compass.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImport_Shapefile->setIcon(icon8);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        mapWidget = new WrapperWidget(centralwidget);
        mapWidget->setObjectName(QStringLiteral("mapWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mapWidget->sizePolicy().hasHeightForWidth());
        mapWidget->setSizePolicy(sizePolicy);
        mapWidget->setMinimumSize(QSize(400, 100));

        horizontalLayout->addWidget(mapWidget);

        horizontalLayout->setStretch(0, 3);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 962, 25));
        menu_Model = new QMenu(menubar);
        menu_Model->setObjectName(QStringLiteral("menu_Model"));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Editor = new QMenu(menubar);
        menu_Editor->setObjectName(QStringLiteral("menu_Editor"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);
        objectMapView = new QDockWidget(MainWindow);
        objectMapView->setObjectName(QStringLiteral("objectMapView"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout_4 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        modelSelector = new QComboBox(dockWidgetContents);
        modelSelector->setObjectName(QStringLiteral("modelSelector"));

        verticalLayout_4->addWidget(modelSelector);

        treeView = new QTreeView(dockWidgetContents);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setHeaderHidden(true);

        verticalLayout_4->addWidget(treeView);

        objectMapView->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), objectMapView);
        consoleView = new QDockWidget(MainWindow);
        consoleView->setObjectName(QStringLiteral("consoleView"));
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents_2);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        saveConsoleButton = new QToolButton(dockWidgetContents_2);
        saveConsoleButton->setObjectName(QStringLiteral("saveConsoleButton"));
        saveConsoleButton->setIcon(icon1);
        saveConsoleButton->setIconSize(QSize(18, 18));

        horizontalLayout_2->addWidget(saveConsoleButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_2);

        console = new QPlainTextEdit(dockWidgetContents_2);
        console->setObjectName(QStringLiteral("console"));

        verticalLayout_2->addWidget(console);

        consoleView->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), consoleView);
        statsView = new QDockWidget(MainWindow);
        statsView->setObjectName(QStringLiteral("statsView"));
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QStringLiteral("dockWidgetContents_3"));
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents_3);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        frame_2 = new QFrame(dockWidgetContents_3);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_6 = new QVBoxLayout(frame_2);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        statsTab = new QTabWidget(frame_2);
        statsTab->setObjectName(QStringLiteral("statsTab"));
        tabStatsHarbours = new QWidget();
        tabStatsHarbours->setObjectName(QStringLiteral("tabStatsHarbours"));
        verticalLayout_7 = new QVBoxLayout(tabStatsHarbours);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        statsTableHarbours = new QTableView(tabStatsHarbours);
        statsTableHarbours->setObjectName(QStringLiteral("statsTableHarbours"));

        verticalLayout_7->addWidget(statsTableHarbours);

        QIcon icon9;
        icon9.addFile(QStringLiteral(":/icons/harbour.png"), QSize(), QIcon::Normal, QIcon::Off);
        statsTab->addTab(tabStatsHarbours, icon9, QString());

        verticalLayout_6->addWidget(statsTab);


        verticalLayout_3->addWidget(frame_2);

        statsView->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), statsView);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QStringLiteral("dockWidget"));
        dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
        dockWidgetContents_4 = new QWidget();
        dockWidgetContents_4->setObjectName(QStringLiteral("dockWidgetContents_4"));
        verticalLayout_5 = new QVBoxLayout(dockWidgetContents_4);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        cmdStart = new QPushButton(dockWidgetContents_4);
        cmdStart->setObjectName(QStringLiteral("cmdStart"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/icons/start.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdStart->setIcon(icon10);

        verticalLayout_5->addWidget(cmdStart);

        cmdPause = new QPushButton(dockWidgetContents_4);
        cmdPause->setObjectName(QStringLiteral("cmdPause"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/icons/pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdPause->setIcon(icon11);

        verticalLayout_5->addWidget(cmdPause);

        cmdStop = new QPushButton(dockWidgetContents_4);
        cmdStop->setObjectName(QStringLiteral("cmdStop"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdStop->setIcon(icon12);

        verticalLayout_5->addWidget(cmdStop);

        cmdSetup = new QPushButton(dockWidgetContents_4);
        cmdSetup->setObjectName(QStringLiteral("cmdSetup"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/icons/setup.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdSetup->setIcon(icon13);

        verticalLayout_5->addWidget(cmdSetup);

        verticalSpacer = new QSpacerItem(20, 201, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);

        groupBox = new QGroupBox(dockWidgetContents_4);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        info_simstep = new QLabel(groupBox);
        info_simstep->setObjectName(QStringLiteral("info_simstep"));
        info_simstep->setMinimumSize(QSize(135, 0));

        verticalLayout->addWidget(info_simstep);


        verticalLayout_5->addWidget(groupBox);

        groupBox_2 = new QGroupBox(dockWidgetContents_4);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 6, 1, 1);

        play_last = new QToolButton(groupBox_2);
        play_last->setObjectName(QStringLiteral("play_last"));
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/icons/last.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_last->setIcon(icon14);

        gridLayout->addWidget(play_last, 1, 5, 1, 1);

        play_first = new QToolButton(groupBox_2);
        play_first->setObjectName(QStringLiteral("play_first"));
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/icons/first.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_first->setIcon(icon15);

        gridLayout->addWidget(play_first, 1, 1, 1, 1);

        play_ffwd = new QToolButton(groupBox_2);
        play_ffwd->setObjectName(QStringLiteral("play_ffwd"));
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/icons/fastfwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_ffwd->setIcon(icon16);

        gridLayout->addWidget(play_ffwd, 0, 5, 1, 1);

        play_fwd = new QToolButton(groupBox_2);
        play_fwd->setObjectName(QStringLiteral("play_fwd"));
        QIcon icon17;
        icon17.addFile(QStringLiteral(":/icons/stepfwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_fwd->setIcon(icon17);

        gridLayout->addWidget(play_fwd, 0, 4, 1, 1);

        play_fbk = new QToolButton(groupBox_2);
        play_fbk->setObjectName(QStringLiteral("play_fbk"));
        QIcon icon18;
        icon18.addFile(QStringLiteral(":/icons/fastrev.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_fbk->setIcon(icon18);

        gridLayout->addWidget(play_fbk, 0, 1, 1, 1);

        play_bk = new QToolButton(groupBox_2);
        play_bk->setObjectName(QStringLiteral("play_bk"));
        QIcon icon19;
        icon19.addFile(QStringLiteral(":/icons/stepbk.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_bk->setIcon(icon19);

        gridLayout->addWidget(play_bk, 0, 2, 1, 1);

        play_step = new QSpinBox(groupBox_2);
        play_step->setObjectName(QStringLiteral("play_step"));
        play_step->setAlignment(Qt::AlignCenter);
        play_step->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(play_step, 1, 2, 1, 3);

        play_auto = new QToolButton(groupBox_2);
        play_auto->setObjectName(QStringLiteral("play_auto"));
        play_auto->setIcon(icon12);

        gridLayout->addWidget(play_auto, 0, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 0, 1, 1);


        verticalLayout_5->addWidget(groupBox_2);

        dockWidget->setWidget(dockWidgetContents_4);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Model->menuAction());
        menubar->addAction(menu_Editor->menuAction());
        menu_Model->addAction(actionConfiguration);
        menu_Model->addAction(actionScenario);
        menu_File->addAction(action_Load);
        menu_File->addAction(actionLoad_results);
        menu_File->addAction(actionImport_results);
        menu_File->addAction(action_Link_database);
        menu_File->addAction(actionImport_Shapefile);
        menu_File->addAction(actionSave);
        menu_File->addSeparator();
        menu_File->addAction(actionQuit);
        menu_Editor->addAction(actionGraph);
        menu_Editor->addAction(actionAdd_Polygon);
        toolBar->addAction(action_Load);
        toolBar->addAction(actionLoad_results);
        toolBar->addAction(actionImport_results);
        toolBar->addAction(action_Link_database);
        toolBar->addAction(actionImport_Shapefile);
        toolBar->addAction(actionSave);
        toolBar->addSeparator();
        toolBar->addAction(actionGraph);
        toolBar->addAction(actionAdd_Polygon);

        retranslateUi(MainWindow);

        statsTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "DISPLACE", 0));
        action_Load->setText(QApplication::translate("MainWindow", "&Load...", 0));
        actionScenario->setText(QApplication::translate("MainWindow", "Scenario...", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save...", 0));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0));
        actionLoad_results->setText(QApplication::translate("MainWindow", "Load results...", 0));
        actionImport_results->setText(QApplication::translate("MainWindow", "Import results...", 0));
        action_Link_database->setText(QApplication::translate("MainWindow", "&Link database...", 0));
        actionConfiguration->setText(QApplication::translate("MainWindow", "Configuration...", 0));
        actionGraph->setText(QApplication::translate("MainWindow", "Graph...", 0));
        actionAdd_Polygon->setText(QApplication::translate("MainWindow", "Add Polygon", 0));
        actionImport_Shapefile->setText(QApplication::translate("MainWindow", "Import Shapefile...", 0));
        menu_Model->setTitle(QApplication::translate("MainWindow", "&Model", 0));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0));
        menu_Editor->setTitle(QApplication::translate("MainWindow", "&Editor", 0));
        objectMapView->setWindowTitle(QApplication::translate("MainWindow", "Model Objects", 0));
        consoleView->setWindowTitle(QApplication::translate("MainWindow", "Console", 0));
        saveConsoleButton->setText(QApplication::translate("MainWindow", "...", 0));
        statsView->setWindowTitle(QApplication::translate("MainWindow", "Statistics", 0));
        statsTab->setTabText(statsTab->indexOf(tabStatsHarbours), QApplication::translate("MainWindow", "Harbours", 0));
        dockWidget->setWindowTitle(QApplication::translate("MainWindow", "Commands", 0));
        cmdStart->setText(QApplication::translate("MainWindow", "Start", 0));
        cmdPause->setText(QApplication::translate("MainWindow", "Pause", 0));
        cmdStop->setText(QApplication::translate("MainWindow", "Stop", 0));
        cmdSetup->setText(QApplication::translate("MainWindow", "Setup", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "Simulation Status", 0));
        info_simstep->setText(QApplication::translate("MainWindow", "Simulation Step:", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Playback", 0));
        play_last->setText(QApplication::translate("MainWindow", "...", 0));
        play_first->setText(QApplication::translate("MainWindow", "...", 0));
        play_ffwd->setText(QApplication::translate("MainWindow", "...", 0));
        play_fwd->setText(QApplication::translate("MainWindow", "...", 0));
        play_fbk->setText(QApplication::translate("MainWindow", "...", 0));
        play_bk->setText(QApplication::translate("MainWindow", "...", 0));
        play_auto->setText(QApplication::translate("MainWindow", "...", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
