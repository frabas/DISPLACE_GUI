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

void MergePopulationDataDialog::inputFileSelected()
{
    mComponent->loadStocksAndSizes(getInputFile(), separator());
}
