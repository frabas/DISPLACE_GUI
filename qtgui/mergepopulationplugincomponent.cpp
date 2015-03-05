#include "mergepopulationplugincomponent.h"
#include "ui_mergepopulationplugincomponent.h"

#include <utils/mrupathmanager.h>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include <QDebug>


MergePopulationPluginComponent::MergePopulationPluginComponent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MergePopulationPluginComponent)
{
    ui->setupUi(this);

    on_expand_toggled(ui->expand->isChecked());
    on_optGeneratePopFile_toggled(false);

    mModel = new TableModel(this);
    ui->stocks->setModel(mModel);
}

MergePopulationPluginComponent::~MergePopulationPluginComponent()
{
    delete ui;
}

void MergePopulationPluginComponent::load(QString file, QChar separator)
{
    mModel->load(file, separator);
}

bool MergePopulationPluginComponent::isOutputStocksChecked() const
{
    return ui->optGeneratePopFile->isChecked();
}

QStringList MergePopulationPluginComponent::getSelectedStocks() const
{
    return mModel->getSelectedStocks();
}

void MergePopulationPluginComponent::selectedNumberChanged()
{
    ui->componentsResume->setText(QString(tr("%1 stocks selected")).arg(mModel->getSelectionNumber()));
}

void MergePopulationPluginComponent::on_browsePopOut_clicked()
{
    MruPathManager man;
    QString path = QFileDialog::getSaveFileName(this, tr("Select stock output file"), man.getMru(MruPathManager::StockNamesFile),
                                                tr("Txt/Dat files (*.txt *.dat);;All files (*.*)"));
    if (!path.isEmpty()) {
        ui->filePopOut->setText(path);
        man.setMru(MruPathManager::StockNamesFile, path);
    }
}

void MergePopulationPluginComponent::on_expand_toggled(bool checked)
{
    ui->stocks->setVisible(checked);
}

void MergePopulationPluginComponent::on_optGeneratePopFile_toggled(bool)
{
    bool v = ui->optGeneratePopFile->isChecked();
    ui->filePopOut->setEnabled(v);
    ui->browsePopOut->setEnabled(v);
}

MergePopulationPluginComponent::TableModel::TableModel(MergePopulationPluginComponent *owner, QObject *parent)
    : QAbstractTableModel(parent), mOwner(owner)
{
}

int MergePopulationPluginComponent::TableModel::rowCount(const QModelIndex &) const
{
    return mList.count();
}

int MergePopulationPluginComponent::TableModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant MergePopulationPluginComponent::TableModel::data(const QModelIndex &index, int role) const
{
    switch (index.column()) {
    case 0:
        if (role == Qt::CheckStateRole)
            return QVariant(mListSelection[index.row()] ? Qt::Checked : Qt::Unchecked);
        break;
    case 1:
        if (role == Qt::DisplayRole)
            return QVariant(mList[index.row()]);
        break;
    }

    return QVariant();
}

bool MergePopulationPluginComponent::TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (index.column()) {
    case 0:
        if (role == Qt::CheckStateRole) {
            mListSelection[index.row()] = value.toInt() == Qt::Checked;
            if (value.toInt() == Qt::Checked) {
                ++mNumSelected;
            } else {
                --mNumSelected;
            }
            emit dataChanged(index, index);
            emit selectionNumberChanged();
        }
        break;
    }

    return false;
}

QVariant MergePopulationPluginComponent::TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return QVariant(section);
    } else {
        switch (section) {
        /*
        case 0:
            if (role == Qt::DisplayRole) {
                return tr("Selection");
            }
            break;
        */
        case 1:
            if (role == Qt::DisplayRole) {
                return tr("Stock");
            }
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags MergePopulationPluginComponent::TableModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;

    return QAbstractTableModel::flags(index);
}

void MergePopulationPluginComponent::TableModel::load(QString filename, QChar separator)
{
    qDebug() << "Loading: " << filename << " sep: " << separator;

    beginResetModel();
    mList.clear();
    mListSelection.clear();
    mNumSelected = 0;
    emit selectionNumberChanged();
    endResetModel();

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot load: " << file.errorString();
        return;
    }

    QString line;

    QTextStream strm(&file);

    line = strm.readLine();
    QStringList fields = line.split(separator, QString::SkipEmptyParts);

    int stock_fld = -1;
    for (int i = 0; i < fields.size(); ++i) {
        if (fields.at(i) == "Stock") {
            stock_fld = i;
            break;
        }
    }

    if (stock_fld == -1) {
        qDebug() << "No field.";
        return;
    }

    QSet<QString> stocks;
    while (!strm.atEnd()) {
        line = strm.readLine();
        fields = line.split(separator, QString::SkipEmptyParts);
        if (stock_fld < fields.size()) {
            stocks.insert(fields.at(stock_fld));
        }
    }

    beginResetModel();
    foreach(QString stock, stocks) {
        mList.append(stock);
        mListSelection.append(true);
        ++mNumSelected;
    }

    qDebug() << mList.size() << " loaded";

    qSort(mList);
    endResetModel();

    emit selectionNumberChanged();
}

int MergePopulationPluginComponent::TableModel::getSelectionNumber() const
{
    return mNumSelected;
}

QStringList MergePopulationPluginComponent::TableModel::getSelectedStocks() const
{
    QStringList r;

    for (int i =0; i < mList.size(); ++i) {
        if (mListSelection.at(i))
            r.append(mList.at(i));
    }

    return r;
}

void MergePopulationPluginComponent::TableModel::selectionNumberChanged()
{
    mOwner->selectedNumberChanged();
}
