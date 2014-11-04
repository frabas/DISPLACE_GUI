#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <config.h>

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class DisplaceModel;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(DisplaceModel *model, QWidget *parent = 0);
    ~ConfigDialog();

    bool get(Config &config) const;
    void set(const Config &config);

private slots:
    void on_readFromTree_clicked();

private:
    Ui::ConfigDialog *ui;
    DisplaceModel *mModel;
};

#endif // CONFIGDIALOG_H
