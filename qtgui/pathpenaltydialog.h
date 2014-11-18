#ifndef PATHPENALTYDIALOG_H
#define PATHPENALTYDIALOG_H

#include <QDialog>

namespace Ui {
class PathPenaltyDialog;
}

class PathPenaltyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPenaltyDialog(QWidget *parent = 0);
    ~PathPenaltyDialog();

    void showShapefileOptions(bool show);
    void setShapefileList(QStringList files);
    QString selectedShapefile() const;

    double weight() const;
private:
    Ui::PathPenaltyDialog *ui;
};

#endif // PATHPENALTYDIALOG_H
