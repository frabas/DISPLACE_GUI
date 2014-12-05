#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <version.h>

#include <QFile>
#include <QTextStream>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QFile f(":/resources/about.txt");
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream s (&f);
        QString txt = s.readAll();

        txt.replace("$VERSION", VER_PRODUCTVERSION_STR);

        ui->aboutText->setTextFormat(Qt::RichText);
        ui->aboutText->setText(txt);
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
