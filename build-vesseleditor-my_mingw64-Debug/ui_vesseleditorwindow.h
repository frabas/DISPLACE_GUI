/********************************************************************************
** Form generated from reading UI file 'vesseleditorwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VESSELEDITORWINDOW_H
#define UI_VESSELEDITORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_VesselEditorWindow
{
public:
    QAction *action_Open;
    QAction *actionGenerate;
    QAction *actionQuit;
    QAction *action_Log_Window;
    QAction *action_Save;
    QAction *actionRScript_location;
    QAction *actionGen_Script_location;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QTableView *table;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QCustomPlot *plot;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *varSelect;
    QLabel *label_2;
    QComboBox *areaSelect;
    QLabel *label_3;
    QComboBox *adimSelect;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_View;
    QStatusBar *statusbar;
    QDockWidget *dockLogWindow;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *clearLog;
    QSpacerItem *horizontalSpacer;
    QPlainTextEdit *logText;

    void setupUi(QMainWindow *VesselEditorWindow)
    {
        if (VesselEditorWindow->objectName().isEmpty())
            VesselEditorWindow->setObjectName(QStringLiteral("VesselEditorWindow"));
        VesselEditorWindow->resize(800, 600);
        action_Open = new QAction(VesselEditorWindow);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/load.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Open->setIcon(icon);
        actionGenerate = new QAction(VesselEditorWindow);
        actionGenerate->setObjectName(QStringLiteral("actionGenerate"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/setup.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGenerate->setIcon(icon1);
        actionQuit = new QAction(VesselEditorWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/quit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon2);
        action_Log_Window = new QAction(VesselEditorWindow);
        action_Log_Window->setObjectName(QStringLiteral("action_Log_Window"));
        action_Log_Window->setCheckable(true);
        action_Save = new QAction(VesselEditorWindow);
        action_Save->setObjectName(QStringLiteral("action_Save"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Save->setIcon(icon3);
        actionRScript_location = new QAction(VesselEditorWindow);
        actionRScript_location->setObjectName(QStringLiteral("actionRScript_location"));
        actionGen_Script_location = new QAction(VesselEditorWindow);
        actionGen_Script_location->setObjectName(QStringLiteral("actionGen_Script_location"));
        centralwidget = new QWidget(VesselEditorWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        table = new QTableView(splitter);
        table->setObjectName(QStringLiteral("table"));
        splitter->addWidget(table);
        frame = new QFrame(splitter);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        plot = new QCustomPlot(frame);
        plot->setObjectName(QStringLiteral("plot"));
        plot->setMinimumSize(QSize(0, 150));

        verticalLayout_2->addWidget(plot);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        varSelect = new QComboBox(frame);
        varSelect->setObjectName(QStringLiteral("varSelect"));

        horizontalLayout->addWidget(varSelect);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        areaSelect = new QComboBox(frame);
        areaSelect->setObjectName(QStringLiteral("areaSelect"));

        horizontalLayout->addWidget(areaSelect);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        adimSelect = new QComboBox(frame);
        adimSelect->setObjectName(QStringLiteral("adimSelect"));

        horizontalLayout->addWidget(adimSelect);


        verticalLayout_2->addLayout(horizontalLayout);

        splitter->addWidget(frame);

        verticalLayout->addWidget(splitter);

        VesselEditorWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(VesselEditorWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_View = new QMenu(menubar);
        menu_View->setObjectName(QStringLiteral("menu_View"));
        VesselEditorWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(VesselEditorWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        VesselEditorWindow->setStatusBar(statusbar);
        dockLogWindow = new QDockWidget(VesselEditorWindow);
        dockLogWindow->setObjectName(QStringLiteral("dockLogWindow"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/Rlogo.png"), QSize(), QIcon::Normal, QIcon::Off);
        dockLogWindow->setWindowIcon(icon4);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        clearLog = new QToolButton(dockWidgetContents);
        clearLog->setObjectName(QStringLiteral("clearLog"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/remove.png"), QSize(), QIcon::Normal, QIcon::Off);
        clearLog->setIcon(icon5);

        horizontalLayout_2->addWidget(clearLog);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout_2);

        logText = new QPlainTextEdit(dockWidgetContents);
        logText->setObjectName(QStringLiteral("logText"));

        verticalLayout_3->addWidget(logText);

        dockLogWindow->setWidget(dockWidgetContents);
        VesselEditorWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockLogWindow);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_View->menuAction());
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Save);
        menu_File->addAction(actionGenerate);
        menu_File->addSeparator();
        menu_File->addAction(actionQuit);
        menu_View->addAction(action_Log_Window);
        menu_View->addSeparator();
        menu_View->addAction(actionRScript_location);
        menu_View->addAction(actionGen_Script_location);

        retranslateUi(VesselEditorWindow);

        QMetaObject::connectSlotsByName(VesselEditorWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VesselEditorWindow)
    {
        VesselEditorWindow->setWindowTitle(QApplication::translate("VesselEditorWindow", "Vessel Creation Editor", 0));
        action_Open->setText(QApplication::translate("VesselEditorWindow", "&Open...", 0));
        actionGenerate->setText(QApplication::translate("VesselEditorWindow", "Generate &outputs...", 0));
        actionQuit->setText(QApplication::translate("VesselEditorWindow", "Quit", 0));
        action_Log_Window->setText(QApplication::translate("VesselEditorWindow", "&Log Window", 0));
        action_Save->setText(QApplication::translate("VesselEditorWindow", "&Save...", 0));
        actionRScript_location->setText(QApplication::translate("VesselEditorWindow", "RScript location...", 0));
        actionGen_Script_location->setText(QApplication::translate("VesselEditorWindow", "Gen Script location...", 0));
        label->setText(QApplication::translate("VesselEditorWindow", "Vid", 0));
        label_2->setText(QApplication::translate("VesselEditorWindow", "Met", 0));
        label_3->setText(QApplication::translate("VesselEditorWindow", "Harb", 0));
        menu_File->setTitle(QApplication::translate("VesselEditorWindow", "&File", 0));
        menu_View->setTitle(QApplication::translate("VesselEditorWindow", "&Utils and Settings", 0));
        dockLogWindow->setWindowTitle(QApplication::translate("VesselEditorWindow", "Log Window", 0));
        clearLog->setText(QApplication::translate("VesselEditorWindow", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class VesselEditorWindow: public Ui_VesselEditorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VESSELEDITORWINDOW_H
