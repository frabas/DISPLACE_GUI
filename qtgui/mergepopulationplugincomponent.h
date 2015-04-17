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

    void loadStocks (QString file, QChar separator);
    void setSizeGroupsCount(int n);

    bool isOutputStocksChecked() const;
    QStringList getSelectedStocks() const;
    QStringList getSelectedSizes() const;

private slots:
    void on_browsePopOut_clicked();
    void on_expand_toggled(bool checked);
    void on_optGeneratePopFile_toggled(bool checked);
    void on_expandSizes_toggled(bool checked);

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

        void load(QString file, QChar separator);
        void setDataCollection (QStringList list);

        int getSelectionCount() const;
        QStringList getSelection() const;

        void stockSelectionNumberChanged();

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
