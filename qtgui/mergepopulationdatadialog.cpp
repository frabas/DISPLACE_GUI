#include "mergepopulationdatadialog.h"

MergePopulationDataDialog::MergePopulationDataDialog(QWidget *parent) :
    MergeDataDialog(parent)
{
    mComponent = new MergePopulationPluginComponent;
    insertPluggableComponent(mComponent);
}

void MergePopulationDataDialog::setSizeGroupCount(int cnt)
{
    mComponent->setSizeGroupsCount(cnt);
}

QStringList MergePopulationDataDialog::getSelectedStocks()
{
    return mComponent->getSelectedStocks();
}

QList<int> MergePopulationDataDialog::getSelectedGroupsIndexes()
{
    return mComponent->getSelectedSizes();
}

bool MergePopulationDataDialog::isPopulationOutChecked()
{
    return mComponent->isOutputStocksChecked();
}

QString MergePopulationDataDialog::getPopulationOutFileName()
{
    return mComponent->getOutputStockFilename();
}

void MergePopulationDataDialog::inputFileSelected()
{
    mComponent->loadStocksAndSizes(getInputFile(), separator());
}
