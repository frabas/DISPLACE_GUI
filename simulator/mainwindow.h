#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "ui_mainwindow.h"
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     QGraphicsScene *scene() ;
     QGraphicsScene *m_scene;
     int timestep;

private slots:
     void on_quitButton_clicked();

     void on_startButton_clicked();


     void on_spinBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;



};

#endif // MAINWINDOW_H
