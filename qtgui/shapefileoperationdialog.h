#ifndef SHAPEFILEOPERATIONDIALOG_H
#define SHAPEFILEOPERATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ShapefileOperationDialog;
}

class ShapefileOperationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShapefileOperationDialog(QWidget *parent = 0);
    ~ShapefileOperationDialog();

    void setShapefileList(QStringList list);
    QString selectedShapefile() const;

private slots:
    void on_radioSelectShapefile_toggled(bool checked);
    void on_radioImportShapefile_toggled(bool checked);
    void on_cmdBrowse_clicked();

private:
    Ui::ShapefileOperationDialog *ui;
};

#endif // SHAPEFILEOPERATIONDIALOG_H
