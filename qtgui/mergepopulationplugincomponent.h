// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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
// --------------------------------------------------------------------------

#ifndef MERGEPOPULATIONPLUGINCOMPONENT_H
#define MERGEPOPULATIONPLUGINCOMPONENT_H

#include <QWidget>
#include <QAbstractTableModel>

namespace Ui {
class MergePopulationPluginComponent;
}

/** \brief a "pluggable" Widget to extends the MergeDataDialog
 * This class extends the functionalities of MergeDataDialog, it is created and hosted in the main UI component.
 * \see MergeDataDialog
 * */
class MergePopulationPluginComponent : public QWidget
{
    Q_OBJECT

public:
    explicit MergePopulationPluginComponent(QWidget *parent = 0);
    ~MergePopulationPluginComponent();

    void loadStocksAndSizes (QString file, QChar separator);
    void setSizeGroupsCount(int n);

    bool isOutputStocksChecked() const;
    QStringList getSelectedStocks() const;
    QList<int> getSelectedSizes() const;

    QString getOutputStockFilename() const;

private slots:
    void on_browsePopOut_clicked();
    void on_expand_toggled(bool checked);
    void on_optGeneratePopFile_toggled(bool checked);
    void on_expandSizes_toggled(bool checked);
    void on_allStocks_clicked();
    void on_noStocks_clicked();
    void on_invStocks_clicked();
    void on_allGroups_clicked();
    void on_noGroups_clicked();
    void on_invGroups_clicked();

private:
    Ui::MergePopulationPluginComponent *ui;

protected:
    class TableModel : public QAbstractTableModel {
    public:
        enum Type {Stocks, Sizes};
        TableModel(MergePopulationPluginComponent *owner, Type type, QObject *parent = 0);

        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        void load(QStringList list, QList<bool> selection);
        void setDataCollection (QStringList list);

        int getSelectionCount() const;
        QStringList getSelection() const;
        QList<int> getSelectionIndexes() const;

        void stockSelectionNumberChanged();

        void selectAll();
        void selectNone();
        void invSelection();
    protected:
        MergePopulationPluginComponent *mOwner = 0;
        Type mType;
        int mNumSelected = 0;

        QStringList mList;
        QList<bool> mListSelection;
    };

    TableModel *mModelStocks;
    TableModel *mModelSizes;

private:
    void selectedStocksNumberChanged(TableModel::Type type);


};

#endif // MERGEPOPULATIONPLUGINCOMPONENT_H
