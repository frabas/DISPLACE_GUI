#ifndef MERGEPOPULATIONDATADIALOG_H
#define MERGEPOPULATIONDATADIALOG_H

#include <mergedatadialog.h>
#include <mergepopulationplugincomponent.h>
#include <QDialog>

/** \brief Implements the UI to merge the population data.
 *
 * This class extends the MergeDataDialog class and mergedatadialog.ui UI by plugging the MergePopulationPluginComponent component.
 * */
class MergePopulationDataDialog : public MergeDataDialog
{
    Q_OBJECT
public:
    explicit MergePopulationDataDialog(QWidget *parent = 0);

    void setSizeGroupCount (int cnt);
signals:

public slots:

protected:
    MergePopulationPluginComponent *mComponent;

    void inputFileSelected();
};

#endif // MERGEPOPULATIONDATADIALOG_H
