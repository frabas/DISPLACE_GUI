#ifndef MERGEPOPULATIONDATADIALOG_H
#define MERGEPOPULATIONDATADIALOG_H

#include <mergedatadialog.h>
#include <mergepopulationplugincomponent.h>
#include <QDialog>

class MergePopulationDataDialog : public MergeDataDialog
{
    Q_OBJECT
public:
    explicit MergePopulationDataDialog(QWidget *parent = 0);

signals:

public slots:

protected:
    MergePopulationPluginComponent *mComponent;

    void inputFileSelected();
};

#endif // MERGEPOPULATIONDATADIALOG_H
