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

    void load (QString file, QChar separator);

    bool isOutputStocksChecked() const;
    QStringList getSelectedStocks() const;

private:
    void selectedNumberChanged();

private slots:
    void on_browsePopOut_clicked();
    void on_expand_toggled(bool checked);

    void on_optGeneratePopFile_toggled(bool checked);

private:
    Ui::MergePopulationPluginComponent *ui;

protected:
    class TableModel : public QAbstractTableModel {
    public:
        TableModel(MergePopulationPluginComponent *owner, QObject *parent = 0);

        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        void load(QString file, QChar separator);

        int getSelectionNumber() const;
        QStringList getSelectedStocks() const;

        void selectionNumberChanged();

    protected:
        MergePopulationPluginComponent *mOwner = 0;
        int mNumSelected = 0;

        QStringList mList;
        QList<bool> mListSelection;
    };

    TableModel *mModel;
};

#endif // MERGEPOPULATIONPLUGINCOMPONENT_H
