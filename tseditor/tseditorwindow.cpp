#include "tseditorwindow.h"
#include "ui_tseditorwindow.h"

TsEditorWindow::TsEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TsEditorWindow)
{
    ui->setupUi(this);
}

TsEditorWindow::~TsEditorWindow()
{
    delete ui;
}
