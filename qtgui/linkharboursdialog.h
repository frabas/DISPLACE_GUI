#ifndef LINKHARBOURSDIALOG_H
#define LINKHARBOURSDIALOG_H

#include <QDialog>

namespace Ui {
class LinkHarboursDialog;
}

class LinkHarboursDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinkHarboursDialog(QWidget *parent = 0);
    ~LinkHarboursDialog();

    bool isRemoveLinksSet() const;
    double getMaxDinstance() const;
    int getMaxLinks() const;
private:
    Ui::LinkHarboursDialog *ui;
};

#endif // LINKHARBOURSDIALOG_H
