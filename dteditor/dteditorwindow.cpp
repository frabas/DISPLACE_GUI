#include "dteditorwindow.h"
#include "ui_dteditorwindow.h"

#include <QCloseEvent>
#include <QSettings>

DtEditorWindow::DtEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DtEditorWindow)
{
    ui->setupUi(this);

    QSettings set;
    restoreGeometry(set.value("mainGeometry").toByteArray());
    restoreState(set.value("mainState").toByteArray());
}

DtEditorWindow::~DtEditorWindow()
{
    delete ui;
}

void DtEditorWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    QSettings sets;
    sets.setValue("mainGeometry", saveGeometry());
    sets.setValue("mainState", saveState());

    qApp->closeAllWindows();
}
