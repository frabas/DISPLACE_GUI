#ifndef CREATEGRAPHDIALOG_H
#define CREATEGRAPHDIALOG_H

#include <QDialog>

namespace Ui {
class CreateGraphDialog;
}

class CreateGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGraphDialog(QWidget *parent = 0);
    ~CreateGraphDialog();

    double step() const;
    double minLon() const;
    double minLat() const;
    double maxLon() const;
    double maxLat() const;

    void setShapefileList(QStringList list);
    QString getSelectedShapefile() const ;

public slots:
    void done(int) override;

private:
    Ui::CreateGraphDialog *ui;
};

#endif // CREATEGRAPHDIALOG_H
