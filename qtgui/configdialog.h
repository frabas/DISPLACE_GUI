#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <config.h>

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

    bool get(Config &config) const;
    void set(const Config &config);

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
