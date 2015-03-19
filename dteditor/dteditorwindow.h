#ifndef DTEDITORWINDOW_H
#define DTEDITORWINDOW_H

#include <QMainWindow>

namespace Ui {
class DtEditorWindow;
}

class DtEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DtEditorWindow(QWidget *parent = 0);
    ~DtEditorWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::DtEditorWindow *ui;
};

#endif // DTEDITORWINDOW_H
