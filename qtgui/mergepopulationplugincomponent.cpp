// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------h------------------------------------------------------

#include "mergepopulationplugincomponent.h"
#include "ui_mergepopulationplugincomponent.h"

#include <workers/populationdistributiondatamergerstrategy.h>

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
    on_expandSizes_toggled(ui->expandSizes->isChecked());
    on_optGeneratePopFile_toggled(false);

    mModelStocks = new TableModel(this, TableModel::Stocks);
    ui->stocks->setModel(mModelStocks);
    mModelSizes = new TableModel(this, TableModel::Sizes);
    ui->sizes->setModel(mModelSizes);

    selectedStocksNumberChanged(TableModel::Stocks);
    selectedStocksNumberChanged(TableModel::Sizes);
}

MergePopulationPluginComponent::~MergePopulationPluginComponent()
{
    delete ui;
}

void MergePopulationPluginComponent::loadStocksAndSizes(QString filename, QChar separator)
{
    qDebug() << "Loading: " << filename << " sep: " << separator;

    QStringList stocklist;
    QStringList sizelist;
    QList<bool> stocksel, sizesel;

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot load: " << file.errorString();
        return;
    }

    QString line;

    QTextStream strm(&file);

    line = strm.readLine();
    QStringList fields = line.split(separator, Qt::SplitBehaviorFlags::SkipEmptyParts);

    int stock_fld = -1;
    for (int i = 0; i < fields.size(); ++i) {
        if (fields.at(i) == displace::workers::PopulationDistributionDataMergerStrategy::StockField) {
            stock_fld = i;
        } else if (fields.at(i).startsWith(displace::workers::PopulationDistributionDataMergerStrategy::IndivFieldPattern)) {
            sizelist << fields.at(i);
            sizesel << true;
        }
    }

    if (stock_fld == -1) {
        qDebug() << "No field.";
        return;
    }

    QSet<QString> stocks;
    while (!strm.atEnd()) {
        line = strm.readLine();
        fields = line.split(separator, Qt::SplitBehaviorFlags::SkipEmptyParts);
        if (stock_fld < fields.size()) {
            stocks.insert(fields.at(stock_fld));
        }
    }

    foreach(QString stock, stocks) {
        stocklist << stock;
        stocksel << true;
    }

    std::sort(stocklist.begin(),stocklist.end());

    mModelStocks->load(stocklist, stocksel);
    mModelSizes->load(sizelist, sizesel);
}

void MergePopulationPluginComponent::setSizeGroupsCount(int n)
{
    QStringList data;
    for (int i = 0; i < n; ++i)
        data.push_back(QString(tr("Size/group #%1")).arg(i+1));
    mModelSizes->setDataCollection(data);
}

bool MergePopulationPluginComponent::isOutputStocksChecked() const
{
    return ui->optGeneratePopFile->isChecked();
}

QStringList MergePopulationPluginComponent::getSelectedStocks() const
{
    return mModelStocks->getSelection();
}

QList<int> MergePopulationPluginComponent::getSelectedSizes() const
{
    return mModelSizes->getSelectionIndexes();
}

QString MergePopulationPluginComponent::getOutputStockFilename() const
{
    return ui->filePopOut->text();
}

void MergePopulationPluginComponent::selectedStocksNumberChanged(TableModel::Type type)
{
    switch (type) {
    case TableModel::Stocks:
        ui->componentsResume->setText(QString(tr("%1 stocks selected")).arg(mModelStocks->getSelectionCount()));
        break;
    case TableModel::Sizes:
        ui->sizeResume->setText(QString(tr("%1 sizes selected")).arg(mModelSizes->getSelectionCount()));
        break;
    }
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
    ui->stocksContainer->setVisible(checked);
}

void MergePopulationPluginComponent::on_expandSizes_toggled(bool checked)
{
    ui->sizesContainer->setVisible(checked);
}

void MergePopulationPluginComponent::on_optGeneratePopFile_toggled(bool)
{
    bool v = ui->optGeneratePopFile->isChecked();
    ui->filePopOut->setEnabled(v);
    ui->browsePopOut->setEnabled(v);
}

MergePopulationPluginComponent::TableModel::TableModel(MergePopulationPluginComponent *owner, Type type, QObject *parent)
    : QAbstractTableModel(parent), mOwner(owner), mType(type)
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
            emit stockSelectionNumberChanged();
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
                switch (mType) {
                case Stocks:
                    return tr("Stock");
                case Sizes:
                    return tr("Sizes");
                }
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

void MergePopulationPluginComponent::TableModel::load(QStringList list, QList<bool> selection)
{
    beginResetModel();
    mList = list;
    mListSelection = selection;
    mNumSelected = mListSelection.count(true);
    endResetModel();

    emit stockSelectionNumberChanged();
}

void MergePopulationPluginComponent::TableModel::setDataCollection(QStringList list)
{
    beginResetModel();
    mList = list;
    mListSelection.clear();
    for (int i = 0; i < mList.size(); ++i)
        mListSelection.push_back(true);
    mNumSelected = mList.size();
    endResetModel();

    emit stockSelectionNumberChanged();
}

int MergePopulationPluginComponent::TableModel::getSelectionCount() const
{
    return mNumSelected;
}

QStringList MergePopulationPluginComponent::TableModel::getSelection() const
{
    QStringList r;

    for (int i =0; i < mList.size(); ++i) {
        if (mListSelection.at(i))
            r.append(mList.at(i));
    }

    return r;
}

QList<int> MergePopulationPluginComponent::TableModel::getSelectionIndexes() const
{
    QList<int> r;

    for (int i =0; i < mList.size(); ++i) {
        if (mListSelection.at(i))
            r.append(i);
    }

    return r;
}

void MergePopulationPluginComponent::TableModel::stockSelectionNumberChanged()
{
    mOwner->selectedStocksNumberChanged(mType);
}

void MergePopulationPluginComponent::TableModel::selectAll()
{
    beginResetModel();
    for (int i = 0; i < mListSelection.size(); ++i) {
        mListSelection[i]=true;
    }
    mNumSelected=mListSelection.size();
    endResetModel();

    emit stockSelectionNumberChanged();
}

void MergePopulationPluginComponent::TableModel::selectNone()
{
    beginResetModel();
    for (int i = 0; i < mListSelection.size(); ++i) {
        mListSelection[i]=false;
    }
    mNumSelected=0;
    endResetModel();

    emit stockSelectionNumberChanged();
}

void MergePopulationPluginComponent::TableModel::invSelection()
{
    beginResetModel();
    for (int i = 0; i < mListSelection.size(); ++i) {
        mListSelection[i]=!mListSelection[i];
    }
    mNumSelected=mListSelection.size() - mNumSelected;
    endResetModel();

    emit stockSelectionNumberChanged();
}


void MergePopulationPluginComponent::on_allStocks_clicked()
{
    mModelStocks->selectAll();
}

void MergePopulationPluginComponent::on_noStocks_clicked()
{
    mModelStocks->selectNone();
}

void MergePopulationPluginComponent::on_invStocks_clicked()
{
    mModelStocks->invSelection();
}

void MergePopulationPluginComponent::on_allGroups_clicked()
{
    mModelSizes->selectAll();
}

void MergePopulationPluginComponent::on_noGroups_clicked()
{
    mModelSizes->selectNone();
}

void MergePopulationPluginComponent::on_invGroups_clicked()
{
    mModelSizes->invSelection();
}
