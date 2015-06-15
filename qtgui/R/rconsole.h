#ifndef RCONSOLE_H
#define RCONSOLE_H

#include <QMainWindow>

namespace Ui {
class RConsole;
}

class QProcess;

class RConsole : public QMainWindow
{
    Q_OBJECT

public:
    explicit RConsole(QWidget *parent = 0);
    ~RConsole();

private slots:
    void on_execute_clicked();
    void on_action_Close_triggered();
    void on_actionExecute_triggered();

public slots:
    void readOutput();

private:
    Ui::RConsole *ui;

    QProcess *mProcess;
};

#endif // RCONSOLE_H
