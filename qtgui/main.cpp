// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "mainwindow.h"

#include <appsettings.h>
#include <QCoreApplication>
#include <QApplication>
#include <ipcqueue.h>

#include <QMapControl/ImageManager.h>

#include <ogrsf_frmts.h>

#include <modelobjects/vesseldata.h>

int main (int argc, char *argv[])
{
    displace::AppSettings::initialize();

    OGRRegisterAll();

    // Cleanup the shm objets
    IpcQueue::forceCleanup();

    // Register here any qmetatype
    qRegisterMetaType<VesselStats>("VesselStats");

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("displace_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    MainWindow mw;

    mw.show();

#ifndef WIN32
    QString path = QDir::homePath() + "/.displace/cache";
#else
    QString path = QDir::homePath() + "/.displace/cache";
#endif

    QDir cachepath(path);
    qmapcontrol::ImageManager::get().enablePersistentCache(std::chrono::minutes(24*60), cachepath);

    return app.exec();
}
