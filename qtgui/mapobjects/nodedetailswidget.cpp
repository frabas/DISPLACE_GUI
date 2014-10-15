#include "nodedetailswidget.h"
#include "ui_nodedetailswidget.h"

NodeDetailsWidget::NodeDetailsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeDetailsWidget)
{
    ui->setupUi(this);
}

NodeDetailsWidget::~NodeDetailsWidget()
{
    delete ui;
}

void NodeDetailsWidget::setText(QString text)
{
    ui->text->setText(text);
}
