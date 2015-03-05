#include "mergepopulationdatadialog.h"

MergePopulationDataDialog::MergePopulationDataDialog(QWidget *parent) :
    MergeDataDialog(parent)
{
    mComponent = new MergePopulationPluginComponent;
    insertPluggableComponent(mComponent);
}

void MergePopulationDataDialog::inputFileSelected()
{
    mComponent->load(getInputFile(), separator());
}
