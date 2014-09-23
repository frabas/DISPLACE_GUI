#include "mainwindow.h"

#include <QCoreApplication>
#include <QApplication>

int main (int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Studiofuga");
    QCoreApplication::setOrganizationDomain("studiofuga.com");
    QCoreApplication::setApplicationName("displace");

    QApplication app(argc, argv);

    MainWindow mw;

    mw.show();

    return app.exec();
}
