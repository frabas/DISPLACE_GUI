#include "csvspecspage.h"
#include "ui_csvspecspage.h"

#include <csv/csvtablemodel.h>
#include <csv/csvimporter.h>
#include <csv/csvexporter.h>

#include <QSortFilterProxyModel>
#include <QMessageBox>

CsvSpecsPage::CsvSpecsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CsvSpecsPage),
    mFilename()
{
    ui->setupUi(this);

    mData = std::make_shared<QList<QStringList>> ();
    mModel = new CsvTableModel(mData);
    mVesselsSpecProxyModel = new QSortFilterProxyModel(this);

    mVesselsSpecProxyModel->setSourceModel(mModel);
    ui->tableView->setModel(mVesselsSpecProxyModel);
}

CsvSpecsPage::~CsvSpecsPage()
{
    delete ui;
}

void CsvSpecsPage::load()
{
    try {
        CsvImporter i;
        i.setSeparator(QChar(';'));
        mData = std::make_shared<QList<QStringList>>(i.import(mFilename));
        mModel->setSource(mData);

        ui->filePath->setText(mFilename);
    } catch (CsvImporter::Exception &x) {
        QMessageBox::warning(this, tr("Load failed"),
                             tr("Cannot load %1: %2").arg(mFilename).arg(x.what()));
        return;
    }

}

void CsvSpecsPage::save()
{
    if (mData == nullptr)
        return;

    try {
        CsvExporter ex;
        ex.setSeparator(QChar(';'));
        ex.exportFile(mFilename, *mData);
    } catch (CsvImporter::Exception &x) {
        QMessageBox::warning(this, tr("Save failed"),
                             tr("Cannot save %1: %2").arg(mFilename).arg(x.what()));
        return;
    }

    QMessageBox::information(this, tr("Save Csv"),
                         tr("File successfully saved to %1").arg(mFilename));

}
