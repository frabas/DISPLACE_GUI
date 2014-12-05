#ifndef CREATESHORTESTPATHDIALOG_H
#define CREATESHORTESTPATHDIALOG_H

#include <QDialog>

namespace Ui {
class CreateShortestPathDialog;
}

class CreateShortestPathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateShortestPathDialog(QWidget *parent = 0);
    ~CreateShortestPathDialog();

    void setShortestPathFolder (QString);
    QString getShortestPathFolder() const;
    void setRelevantNodesFolder(QString);
    QString getRelevantNodesFolder() const;
    bool isAllNodesAreRelevantChecked() const;

private slots:
    void on_browseShortestFolder_clicked();
    void on_browseRelevantFolder_clicked();

private:
    Ui::CreateShortestPathDialog *ui;
};

#endif // CREATESHORTESTPATHDIALOG_H
