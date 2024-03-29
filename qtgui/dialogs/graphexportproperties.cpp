#include "graphexportproperties.h"
#include "ui_graphexportproperties.h"

#include <utils/imageformathelpers.h>

#include <QSettings>

GraphExportProperties::GraphExportProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphExportProperties)
{
    ui->setupUi(this);

    QSettings s;
    ui->width->setValue(s.value("export_all_graphs_width", 1024).toInt());
    ui->height->setValue(s.value("export_all_graphs_height", 768).toInt());

    QString lastform = s.value("ImageExport.format", "png").toString();
    QStringList filter = displace::helpers::images::supportedFormatsOnWrite();

    ui->fileType->addItems(filter);

    int idx = displace::helpers::images::supportedFormatsOnWrite().indexOf(lastform);
    if (idx != -1)
        ui->fileType->setCurrentIndex(idx);
}

GraphExportProperties::~GraphExportProperties()
{
    delete ui;
}

GraphProperties GraphExportProperties::getOptions() const
{
    GraphProperties p;
    p.width = ui->width->value();
    p.height = ui->height->value();
    p.format = ui->fileType->currentText();

    return p;
}

void GraphExportProperties::on_width_valueChanged(int wi)
{
    ui->aspect->setValue((float)wi / (float)ui->height->value());
}

void GraphExportProperties::on_height_valueChanged(int he)
{
    ui->aspect->setValue((float)ui->width->value() / (float)he);
}


void GraphExportProperties::on_aspect_valueChanged(double ratio)
{
    ui->height->setValue(ui->width->value() / ratio);
}

void GraphExportProperties::accept()
{
    QSettings s;
    s.setValue("export_all_graphs_width", ui->width->value());
    s.setValue("export_all_graphs_height", ui->height->value());

    QDialog::accept();
}

