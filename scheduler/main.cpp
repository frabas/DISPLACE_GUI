#include "mainwindow.h"
#include <QApplication>
#include <appsettings.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    displace::AppSettings::initialize();

    MainWindow w;
    w.show();

    return a.exec();
}
