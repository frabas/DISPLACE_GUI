#ifndef SAVEGRAPHDIALOG_H
#define SAVEGRAPHDIALOG_H

#include <QDialog>

namespace Ui {
class SaveGraphDialog;
}

class SaveGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveGraphDialog(QWidget *parent = 0);
    ~SaveGraphDialog();

    void setName(QString name);
    void setOutputFolder(QString path);
    QString getGraphFilename() const;
    QString getCoordsFilename() const;
    QString getLandscapeFilename() const;
    QString getAreacodesFilename() const;
    QString getClosedPolygonFilename() const;
    bool isClosedPolygonExportChecked() const;

private slots:
    void on_optCustomize_toggled(bool checked);
    void on_graphName_textChanged(const QString &arg1);
    void on_cmdBrowse_clicked();

private:
    Ui::SaveGraphDialog *ui;
};

#endif // SAVEGRAPHDIALOG_H
