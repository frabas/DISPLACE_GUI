#ifndef TSEDITORWINDOW_H
#define TSEDITORWINDOW_H

#include <QMainWindow>

namespace Ui {
class TsEditorWindow;
}

class TsEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TsEditorWindow(QWidget *parent = 0);
    ~TsEditorWindow();

private:
    Ui::TsEditorWindow *ui;
};

#endif // TSEDITORWINDOW_H
