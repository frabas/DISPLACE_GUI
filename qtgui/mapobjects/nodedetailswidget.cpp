#include "nodedetailswidget.h"
#include "ui_nodedetailswidget.h"

NodeDetailsWidget::NodeDetailsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeDetailsWidget)
{
    ui->setupUi(this);
    ui->tool1->setVisible(false);

    connect (ui->tool1, SIGNAL(clicked()), this, SIGNAL(toolButtonClicked()));
}

NodeDetailsWidget::~NodeDetailsWidget()
{
    delete ui;
}

void NodeDetailsWidget::setText(QString text)
{
    ui->text->setText(text);
}

void NodeDetailsWidget::showTool(bool display)
{
    ui->tool1->setVisible(display);
}
