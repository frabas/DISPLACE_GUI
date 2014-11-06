/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
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
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"
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
    QAction *actionImport_Shapefile;
    QAction *actionPalettes;
    QAction *actionPopulations;
    QAction *actionEdge_Edit;
    QAction *actionClear_Graph;
    QAction *actionAdd;
    QAction *actionDelete;
    QAction *actionProperties;
    QAction *actionNode_Editor;
    QAction *actionExport_Graph;
    QAction *actionSave_Graph;
    QAction *actionLoad_Graph;
    QAction *actionCreate_Graph;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    WrapperWidget *mapWidget;
    QMenuBar *menubar;
    QMenu *menu_Model;
    QMenu *menuPalettes;
    QMenu *menu_File;
    QMenu *menu_Editor;
    QMenu *menuGraph;
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
    QHBoxLayout *horizontalLayout_5;
    QComboBox *harbStatSelector;
    QSpacerItem *horizontalSpacer_6;
    QCustomPlot *plotHarbours;
    QWidget *tabStatsPopulations;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *popStatSelector;
    QSpacerItem *horizontalSpacer_4;
    QCustomPlot *plotPopulations;
    QWidget *nationsTab;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *nationsStatsSelector;
    QSpacerItem *horizontalSpacer_5;
    QCustomPlot *plotNations;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents_4;
    QVBoxLayout *verticalLayout_5;
    QPushButton *cmdStart;
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
    QToolButton *play_params;
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
        actionGraph->setCheckable(true);
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/graph.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGraph->setIcon(icon6);
        actionImport_Shapefile = new QAction(MainWindow);
        actionImport_Shapefile->setObjectName(QStringLiteral("actionImport_Shapefile"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icons/compass.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImport_Shapefile->setIcon(icon7);
        actionPalettes = new QAction(MainWindow);
        actionPalettes->setObjectName(QStringLiteral("actionPalettes"));
        actionPopulations = new QAction(MainWindow);
        actionPopulations->setObjectName(QStringLiteral("actionPopulations"));
        actionEdge_Edit = new QAction(MainWindow);
        actionEdge_Edit->setObjectName(QStringLiteral("actionEdge_Edit"));
        actionEdge_Edit->setCheckable(true);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/icons/edge.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionEdge_Edit->setIcon(icon8);
        actionClear_Graph = new QAction(MainWindow);
        actionClear_Graph->setObjectName(QStringLiteral("actionClear_Graph"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/icons/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClear_Graph->setIcon(icon9);
        actionAdd = new QAction(MainWindow);
        actionAdd->setObjectName(QStringLiteral("actionAdd"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/icons/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAdd->setIcon(icon10);
        actionDelete = new QAction(MainWindow);
        actionDelete->setObjectName(QStringLiteral("actionDelete"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/icons/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon11);
        actionProperties = new QAction(MainWindow);
        actionProperties->setObjectName(QStringLiteral("actionProperties"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/icons/info.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionProperties->setIcon(icon12);
        actionNode_Editor = new QAction(MainWindow);
        actionNode_Editor->setObjectName(QStringLiteral("actionNode_Editor"));
        actionNode_Editor->setCheckable(true);
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/icons/node.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNode_Editor->setIcon(icon13);
        actionExport_Graph = new QAction(MainWindow);
        actionExport_Graph->setObjectName(QStringLiteral("actionExport_Graph"));
        actionSave_Graph = new QAction(MainWindow);
        actionSave_Graph->setObjectName(QStringLiteral("actionSave_Graph"));
        actionLoad_Graph = new QAction(MainWindow);
        actionLoad_Graph->setObjectName(QStringLiteral("actionLoad_Graph"));
        actionCreate_Graph = new QAction(MainWindow);
        actionCreate_Graph->setObjectName(QStringLiteral("actionCreate_Graph"));
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

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 962, 25));
        menu_Model = new QMenu(menubar);
        menu_Model->setObjectName(QStringLiteral("menu_Model"));
        menuPalettes = new QMenu(menu_Model);
        menuPalettes->setObjectName(QStringLiteral("menuPalettes"));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Editor = new QMenu(menubar);
        menu_Editor->setObjectName(QStringLiteral("menu_Editor"));
        menuGraph = new QMenu(menubar);
        menuGraph->setObjectName(QStringLiteral("menuGraph"));
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
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        harbStatSelector = new QComboBox(tabStatsHarbours);
        harbStatSelector->setObjectName(QStringLiteral("harbStatSelector"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(harbStatSelector->sizePolicy().hasHeightForWidth());
        harbStatSelector->setSizePolicy(sizePolicy1);

        horizontalLayout_5->addWidget(harbStatSelector);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);


        verticalLayout_7->addLayout(horizontalLayout_5);

        plotHarbours = new QCustomPlot(tabStatsHarbours);
        plotHarbours->setObjectName(QStringLiteral("plotHarbours"));

        verticalLayout_7->addWidget(plotHarbours);

        verticalLayout_7->setStretch(1, 1);
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/icons/harbour.png"), QSize(), QIcon::Normal, QIcon::Off);
        statsTab->addTab(tabStatsHarbours, icon14, QString());
        tabStatsPopulations = new QWidget();
        tabStatsPopulations->setObjectName(QStringLiteral("tabStatsPopulations"));
        verticalLayout_8 = new QVBoxLayout(tabStatsPopulations);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        popStatSelector = new QComboBox(tabStatsPopulations);
        popStatSelector->setObjectName(QStringLiteral("popStatSelector"));
        sizePolicy1.setHeightForWidth(popStatSelector->sizePolicy().hasHeightForWidth());
        popStatSelector->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(popStatSelector);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout_8->addLayout(horizontalLayout_3);

        plotPopulations = new QCustomPlot(tabStatsPopulations);
        plotPopulations->setObjectName(QStringLiteral("plotPopulations"));

        verticalLayout_8->addWidget(plotPopulations);

        verticalLayout_8->setStretch(1, 1);
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/icons/fish.png"), QSize(), QIcon::Normal, QIcon::Off);
        statsTab->addTab(tabStatsPopulations, icon15, QString());
        nationsTab = new QWidget();
        nationsTab->setObjectName(QStringLiteral("nationsTab"));
        verticalLayout_9 = new QVBoxLayout(nationsTab);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        nationsStatsSelector = new QComboBox(nationsTab);
        nationsStatsSelector->setObjectName(QStringLiteral("nationsStatsSelector"));
        sizePolicy1.setHeightForWidth(nationsStatsSelector->sizePolicy().hasHeightForWidth());
        nationsStatsSelector->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(nationsStatsSelector);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);


        verticalLayout_9->addLayout(horizontalLayout_4);

        plotNations = new QCustomPlot(nationsTab);
        plotNations->setObjectName(QStringLiteral("plotNations"));

        verticalLayout_9->addWidget(plotNations);

        verticalLayout_9->setStretch(1, 1);
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/icons/places.png"), QSize(), QIcon::Normal, QIcon::Off);
        statsTab->addTab(nationsTab, icon16, QString());

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
        QIcon icon17;
        icon17.addFile(QStringLiteral(":/icons/start.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdStart->setIcon(icon17);

        verticalLayout_5->addWidget(cmdStart);

        cmdStop = new QPushButton(dockWidgetContents_4);
        cmdStop->setObjectName(QStringLiteral("cmdStop"));
        QIcon icon18;
        icon18.addFile(QStringLiteral(":/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdStop->setIcon(icon18);

        verticalLayout_5->addWidget(cmdStop);

        cmdSetup = new QPushButton(dockWidgetContents_4);
        cmdSetup->setObjectName(QStringLiteral("cmdSetup"));
        QIcon icon19;
        icon19.addFile(QStringLiteral(":/icons/setup.png"), QSize(), QIcon::Normal, QIcon::Off);
        cmdSetup->setIcon(icon19);

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
        QIcon icon20;
        icon20.addFile(QStringLiteral(":/icons/last.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_last->setIcon(icon20);

        gridLayout->addWidget(play_last, 1, 5, 1, 1);

        play_first = new QToolButton(groupBox_2);
        play_first->setObjectName(QStringLiteral("play_first"));
        QIcon icon21;
        icon21.addFile(QStringLiteral(":/icons/first.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_first->setIcon(icon21);

        gridLayout->addWidget(play_first, 1, 1, 1, 1);

        play_ffwd = new QToolButton(groupBox_2);
        play_ffwd->setObjectName(QStringLiteral("play_ffwd"));
        QIcon icon22;
        icon22.addFile(QStringLiteral(":/icons/fastfwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_ffwd->setIcon(icon22);

        gridLayout->addWidget(play_ffwd, 0, 5, 1, 1);

        play_fwd = new QToolButton(groupBox_2);
        play_fwd->setObjectName(QStringLiteral("play_fwd"));
        QIcon icon23;
        icon23.addFile(QStringLiteral(":/icons/stepfwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_fwd->setIcon(icon23);

        gridLayout->addWidget(play_fwd, 0, 4, 1, 1);

        play_fbk = new QToolButton(groupBox_2);
        play_fbk->setObjectName(QStringLiteral("play_fbk"));
        QIcon icon24;
        icon24.addFile(QStringLiteral(":/icons/fastrev.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_fbk->setIcon(icon24);

        gridLayout->addWidget(play_fbk, 0, 1, 1, 1);

        play_bk = new QToolButton(groupBox_2);
        play_bk->setObjectName(QStringLiteral("play_bk"));
        QIcon icon25;
        icon25.addFile(QStringLiteral(":/icons/stepbk.png"), QSize(), QIcon::Normal, QIcon::Off);
        play_bk->setIcon(icon25);

        gridLayout->addWidget(play_bk, 0, 2, 1, 1);

        play_step = new QSpinBox(groupBox_2);
        play_step->setObjectName(QStringLiteral("play_step"));
        play_step->setAlignment(Qt::AlignCenter);
        play_step->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(play_step, 1, 2, 1, 3);

        play_auto = new QToolButton(groupBox_2);
        play_auto->setObjectName(QStringLiteral("play_auto"));
        play_auto->setIcon(icon17);

        gridLayout->addWidget(play_auto, 0, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 0, 1, 1);

        play_params = new QToolButton(groupBox_2);
        play_params->setObjectName(QStringLiteral("play_params"));

        gridLayout->addWidget(play_params, 2, 5, 1, 1);


        verticalLayout_5->addWidget(groupBox_2);

        dockWidget->setWidget(dockWidgetContents_4);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Model->menuAction());
        menubar->addAction(menuGraph->menuAction());
        menubar->addAction(menu_Editor->menuAction());
        menu_Model->addAction(actionConfiguration);
        menu_Model->addAction(actionScenario);
        menu_Model->addAction(menuPalettes->menuAction());
        menuPalettes->addAction(actionPalettes);
        menuPalettes->addAction(actionPopulations);
        menu_File->addAction(action_Load);
        menu_File->addAction(actionLoad_results);
        menu_File->addAction(actionImport_results);
        menu_File->addAction(actionImport_Shapefile);
        menu_File->addAction(action_Link_database);
        menu_File->addAction(actionSave);
        menu_File->addSeparator();
        menu_File->addAction(actionQuit);
        menu_Editor->addAction(actionGraph);
        menu_Editor->addAction(actionClear_Graph);
        menu_Editor->addAction(actionNode_Editor);
        menu_Editor->addAction(actionEdge_Edit);
        menu_Editor->addSeparator();
        menu_Editor->addAction(actionAdd);
        menu_Editor->addAction(actionDelete);
        menu_Editor->addAction(actionProperties);
        menuGraph->addAction(actionLoad_Graph);
        menuGraph->addAction(actionSave_Graph);
        menuGraph->addAction(actionExport_Graph);
        menuGraph->addSeparator();
        menuGraph->addAction(actionCreate_Graph);
        toolBar->addAction(action_Load);
        toolBar->addAction(actionLoad_results);
        toolBar->addAction(actionImport_results);
        toolBar->addAction(action_Link_database);
        toolBar->addAction(actionImport_Shapefile);
        toolBar->addAction(actionSave);
        toolBar->addSeparator();
        toolBar->addAction(actionGraph);
        toolBar->addAction(actionNode_Editor);
        toolBar->addAction(actionEdge_Edit);
        toolBar->addSeparator();
        toolBar->addAction(actionAdd);
        toolBar->addAction(actionDelete);
        toolBar->addAction(actionProperties);

        retranslateUi(MainWindow);

        statsTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "DISPLACE", 0));
        action_Load->setText(QApplication::translate("MainWindow", "&Load...", 0));
        actionScenario->setText(QApplication::translate("MainWindow", "Scenario...", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save Simulation Config...", 0));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0));
        actionLoad_results->setText(QApplication::translate("MainWindow", "Load results...", 0));
        actionImport_results->setText(QApplication::translate("MainWindow", "Import results...", 0));
        action_Link_database->setText(QApplication::translate("MainWindow", "&Link database...", 0));
        actionConfiguration->setText(QApplication::translate("MainWindow", "Configuration...", 0));
        actionGraph->setText(QApplication::translate("MainWindow", "Editor Mode", 0));
        actionImport_Shapefile->setText(QApplication::translate("MainWindow", "Import Shapefile...", 0));
        actionPalettes->setText(QApplication::translate("MainWindow", "Values...", 0));
        actionPopulations->setText(QApplication::translate("MainWindow", "Populations...", 0));
        actionEdge_Edit->setText(QApplication::translate("MainWindow", "Edge Editor", 0));
        actionClear_Graph->setText(QApplication::translate("MainWindow", "Clear Graph", 0));
        actionAdd->setText(QApplication::translate("MainWindow", "Add...", 0));
        actionDelete->setText(QApplication::translate("MainWindow", "Delete...", 0));
        actionDelete->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0));
        actionProperties->setText(QApplication::translate("MainWindow", "Properties...", 0));
        actionNode_Editor->setText(QApplication::translate("MainWindow", "Node Editor", 0));
        actionExport_Graph->setText(QApplication::translate("MainWindow", "Export...", 0));
        actionSave_Graph->setText(QApplication::translate("MainWindow", "Save...", 0));
        actionLoad_Graph->setText(QApplication::translate("MainWindow", "Load...", 0));
        actionCreate_Graph->setText(QApplication::translate("MainWindow", "Create Graph...", 0));
        menu_Model->setTitle(QApplication::translate("MainWindow", "&Model", 0));
        menuPalettes->setTitle(QApplication::translate("MainWindow", "Palettes", 0));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0));
        menu_Editor->setTitle(QApplication::translate("MainWindow", "&Editor", 0));
        menuGraph->setTitle(QApplication::translate("MainWindow", "Graph", 0));
        objectMapView->setWindowTitle(QApplication::translate("MainWindow", "Model Objects", 0));
        consoleView->setWindowTitle(QApplication::translate("MainWindow", "Console", 0));
        saveConsoleButton->setText(QApplication::translate("MainWindow", "...", 0));
        statsView->setWindowTitle(QApplication::translate("MainWindow", "Statistics", 0));
        harbStatSelector->clear();
        harbStatSelector->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Cum Catches", 0)
         << QApplication::translate("MainWindow", "Profit", 0)
        );
        statsTab->setTabText(statsTab->indexOf(tabStatsHarbours), QApplication::translate("MainWindow", "Harbours", 0));
        popStatSelector->clear();
        popStatSelector->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Number at size", 0)
         << QApplication::translate("MainWindow", "Fishing mortality", 0)
        );
        statsTab->setTabText(statsTab->indexOf(tabStatsPopulations), QApplication::translate("MainWindow", "Populations", 0));
        nationsStatsSelector->clear();
        nationsStatsSelector->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Catches", 0)
         << QApplication::translate("MainWindow", "Revenues", 0)
         << QApplication::translate("MainWindow", "Time At Sea", 0)
        );
        statsTab->setTabText(statsTab->indexOf(nationsTab), QApplication::translate("MainWindow", "Nations", 0));
        dockWidget->setWindowTitle(QApplication::translate("MainWindow", "Commands", 0));
        cmdStart->setText(QApplication::translate("MainWindow", "Start", 0));
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
        play_params->setText(QApplication::translate("MainWindow", "...", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
