#ifndef RCONSOLE_H
#define RCONSOLE_H

#include <QMainWindow>

namespace Ui {
class RConsole;
}

class RConsole : public QMainWindow
{
    Q_OBJECT

public:
    explicit RConsole(QWidget *parent = 0);
    ~RConsole();

private slots:
    void on_execute_clicked();

private:
    Ui::RConsole *ui;
};

#endif // RCONSOLE_H
