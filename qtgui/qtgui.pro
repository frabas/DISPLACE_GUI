TEMPLATE=app
QT += core gui network sql xml printsupport concurrent
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = displacegui

macx {
    TARGET=$$APPTARGET
    TEMPLATE=app
    DESTDIR=$$APPDESTDIR

    QMAKE_INFO_PLIST = resources/Info.plist
    ICON = icons/displace.icns

    #CONFIG -= app_bundle

    ### qmake 4.7.4 workaround
#    QMAKE_INFO_PLIST_OUT = $$DESTDIR/$$TARGETAPPBUNDLE/Contents/Info.plist

#    missing.target = dummy
#    missing.depends = $$DESTDIR/$$TARGETAPPBUNDLE/Contents/Info.plist $$DESTDIR/$$TARGETAPPBUNDLE/Contents/Resources/displace.icns

#    QMAKE_EXTRA_TARGETS = missing
#    QMAKE_PRE_LINK = make dummy
}

INCLUDEPATH+=../include/ ../QMapControl/QMapControl/src/ ../commons ../qtcommons ../formats ../sparsepp

include (../QMapControl/QMapControl/QMapControl.pri)
include ("$$top_srcdir/localconfig.pri")

DESTDIR = ../
macx {
    DESTDIR=$$APPDESTDIR
    TARGET=$$APPTARGET
}

QMAPCONTROL_LIB=qmapcontrol

CONFIG(debug,release|debug) {
    DEFINES += DEBUG
    QMAPCONTROL_LIB=qmapcontrold
}
CONFIG(release,release|debug) {
    QMAPCONTROL_LIB=qmapcontrol

# For use with debugger in release mode
#   QMAKE_CXXFLAGS_RELEASE += -g -O2
#   QMAKE_LFLAGS_RELEASE=
}

GEOGRAPHICLIB=Geographic

win32 {
    RC_FILE = qtgui.rc
    QMAPCONTROL_LIB=$${QMAPCONTROL_LIB}1

    CGAL_LIBS= -lCGAL-vc140-mt-4.9 -lgmp -lboost_system$${BOOST_SUFFIX}
    CONFIG(debug,release|debug){
        GEOGRAPHICLIB=Geographic_d
    }
}

unix {
    CGAL_LIBS= -lCGAL -lgmp
}


DEFINES += HAVE_GEOGRAPHICLIB
LIBS += -l$$GEOGRAPHICLIB

!macx {
    QMAKE_CXXFLAGS += -frounding-math
    DEFINES += PROFILE
}

LIBS+=-L.. -lformats -ldisplacecommons -lqtcommons -l$$QMAPCONTROL_LIB $$CGAL_LIBS

# Add GDAL include path.
INCLUDEPATH += $$QMC_GDAL_INC

# Add GDAL library path and library (windows).
win32:LIBS += -L$$QMC_GDAL_LIB -lgdal_i

# Add GDAL library path and library (unix).
unix:LIBS += -lgdal

#macx:LIBS += -L/Library/Frameworks/GDAL.framework/unix/lib/

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    objecttreemodel.cpp \
    displacemodel.cpp \
    simulator.cpp \
    objects/objecttreeentity.cpp \
    objects/layerentity.cpp \
    objects/harbourentity.cpp \
    scenario.cpp \
    scenariodialog.cpp \
    config.cpp \
    objects/nodeentity.cpp \
    mapobjects/harbourmapobject.cpp \
    mapobjects/mapobject.cpp \
    mapobjectscontroller.cpp \
    mapobjects/nodemapobject.cpp \
    objects/vesselentity.cpp \
    mapobjects/vesselmapobject.cpp \
    objects/benthosentity.cpp \
    modelobjects/benthos.cpp \
    outputfileparser.cpp \
    simulationsetupdialog.cpp \
    dbhelper.cpp \
    modelobjects/nodedata.cpp \
    modelobjects/vesseldata.cpp \
    mapobjects/nodegraphics.cpp \
    editpalettedialog.cpp \
    palettemanager.cpp \
    statscontroller.cpp \
    modelobjects/populationdata.cpp \
    objects/populationentity.cpp \
    graphinteractioncontroller.cpp \
    modelobjects/nationdata.cpp \
    objects/nationentity.cpp \
    objects/szgroupentity.cpp \
    configdialog.cpp \
    mapobjects/nodedetailswidget.cpp \
    modelobjects/harbourdata.cpp \
    mapobjects/edgemapobject.cpp \
    mapobjects/edgelayer.cpp \
    creategraphdialog.cpp \
    graphbuilder.cpp \
    calendar.cpp \
    waitdialog.cpp \
    backgroundworker.cpp \
    inputfileparser.cpp \
    shortestpathbuilder.cpp \
    mousemode.cpp \
    mousemode/drawpenaltypolygon.cpp \
    pathpenaltydialog.cpp \
    inputfileexporter.cpp \
    linkharboursdialog.cpp \
    mousemode/movefilteringmousemodedecorator.cpp \
    mousemode/singleclickmousemode.cpp \
    mousemode/edgeaddmousemode.cpp \
    aboutdialog.cpp \
    createshortestpathdialog.cpp \
    simulatoripcmanager.cpp \
    workers/shortestpathbuilderworker.cpp \
    csveditor.cpp \
    mergedatadialog.cpp \
    workers/datamerger.cpp \
    utils/imageformathelpers.cpp \
    objects/shapefilelayerentity.cpp \
    utils/fileformathelper.cpp \
    shapefileoperationdialog.cpp \
    utils/mrupathmanager.cpp \
    savegraphdialog.cpp \
    workers/populationdistributiondatamergerstrategy.cpp \
    mergepopulationdatadialog.cpp \
    mergepopulationplugincomponent.cpp \
    workers/mergerstrategy.cpp \
    workers/graphbuilderworker.cpp \
    algo/isolatedsubgraphchecker.cpp \
    algo/geographicgridbuilder.cpp \
    algo/simplegeodesiclinegraphbuilder.cpp \
    algo/simpleplanargraphbuilder.cpp \
    R/rconsole.cpp \
    modelobjects/fishfarmdata.cpp \
    objects/fishfarmentity.cpp \
    mapobjects/fishfarmobject.cpp \
    modelobjects/shipdata.cpp \
    mapobjects/shipmapobject.cpp \
    objects/shipentity.cpp \
    objects/metiersentity.cpp \
    dialogs/graphexportproperties.cpp \
    objects/windmillentity.cpp \
    modelobjects/windmilldata.cpp \
    mapobjects/windmillobject.cpp \
    graphbuilder_shp.cpp \
    objects/firmentity.cpp \
    modelobjects/firmdata.cpp \
    mapobjects/firmmapobject.cpp \
    objects/bfunctionalgroupsentity.cpp \
    plots/benthosstatsplot.cpp \
    stats/benthosstats.cpp \
    stats/fishfarmsstats.cpp \
    plots/fishfarmsstatsplot.cpp \
    objects/ffarmtypesentity.cpp \
    stats/shipsstats.cpp \
    stats/windfarmsstats.cpp \
    plots/windfarmsstatsplot.cpp \
    plots/shipsstatsplot.cpp \
    objects/wfarmtypesentity.cpp \
    objects/shiptypesentity.cpp \
    plots/nationsstatsplot.cpp \
    plots/populationsstatplot.cpp \
    plots/harboursstatplot.cpp \
    plots/metiersstatsplot.cpp \
    mapsdataprovider.cpp \
    simplenoninterestingnodesgraphsimplifier.cpp \
    plots/vesselsstatsplot.cpp \
    plots/BaseStatsPlotImpl.cpp \
    algo/AStarShortestPath.cpp \
    algo/DijkstraShortestPath.cpp

FORMS += \
    mainwindow.ui \
    scenariodialog.ui \
    simulationsetupdialog.ui \
    editpalettedialog.ui \
    configdialog.ui \
    mapobjects/nodedetailswidget.ui \
    creategraphdialog.ui \
    waitdialog.ui \
    pathpenaltydialog.ui \
    linkharboursdialog.ui \
    aboutdialog.ui \
    createshortestpathdialog.ui \
    csveditor.ui \
    mergedatadialog.ui \
    shapefileoperationdialog.ui \
    savegraphdialog.ui \
    mergepopulationplugincomponent.ui \
    R/rconsole.ui \
    dialogs/graphexportproperties.ui

HEADERS += \
    mainwindow.h \
    objecttreemodel.h \
    displacemodel.h \
    simulator.h \
    objects/objecttreeentity.h \
    objects/layerentity.h \
    objects/harbourentity.h \
    scenario.h \
    exceptions.h \
    scenariodialog.h \
    config.h \
    objects/nodeentity.h \
    mapobjects/harbourmapobject.h \
    mapobjects/mapobject.h \
    mapobjectscontroller.h \
    mapobjects/nodemapobject.h \
    objects/vesselentity.h \
    mapobjects/vesselmapobject.h \
    objects/benthosentity.h \
    modelobjects/benthos.h \
    outputfileparser.h \
    simulationsetupdialog.h \
    dbhelper.h \
    modelobjects/nodedata.h \
    modelobjects/vesseldata.h \
    mapobjects/nodegraphics.h \
    editpalettedialog.h \
    palettemanager.h \
    statscontroller.h \
    modelobjects/populationdata.h \
    objects/populationentity.h \
    ../include/profiler.h \
    historicaldatacollector.h \
    graphinteractioncontroller.h \
    modelobjects/nationdata.h \
    objects/nationentity.h \
    objects/szgroupentity.h \
    configdialog.h \
    mapobjects/nodedetailswidget.h \
    modelobjects/harbourdata.h \
    mapobjects/edgemapobject.h \
    mapobjects/edgelayer.h \
    ../include/memoryinfo.h \
    creategraphdialog.h \
    graphbuilder.h \
    calendar.h \
    waitdialog.h \
    backgroundworker.h \
    inputfileparser.h \
    shortestpathbuilder.h \
    mousemode.h \
    mousemode/drawpenaltypolygon.h \
    editorlayerinterface.h \
    pathpenaltydialog.h \
    inputfileexporter.h \
    linkharboursdialog.h \
    mousemode/movefilteringmousemodedecorator.h \
    mousemode/singleclickmousemode.h \
    mousemode/edgeaddmousemode.h \
    ../include/version.h \
    aboutdialog.h \
    createshortestpathdialog.h \
    simulatoripcmanager.h \
    workers/shortestpathbuilderworker.h \
    csveditor.h \
    mergedatadialog.h \
    workers/datamerger.h \
    utils/imageformathelpers.h \
    objects/shapefilelayerentity.h \
    utils/fileformathelper.h \
    shapefileoperationdialog.h \
    utils/mrupathmanager.h \
    savegraphdialog.h \
    workers/populationdistributiondatamergerstrategy.h \
    utils/displaceexception.h \
    mergepopulationdatadialog.h \
    mergepopulationplugincomponent.h \
    workers/mergerstrategy.h \
    workers/graphbuilderworker.h \
    algo/isolatedsubgraphchecker.h \
    mapobjects/mapobjectcontainer.h \
    algo/geographicgridbuilder.h \
    algo/simplegeodesiclinegraphbuilder.h \
    algo/simpleplanargraphbuilder.h \
    R/rconsole.h \
    modelobjects/fishfarmdata.h \
    objects/fishfarmentity.h \
    mapobjects/fishfarmobject.h \
    modelobjects/shipdata.h \
    mapobjects/shipmapobject.h \
    objects/shipentity.h \
    modelobjects/metierdata.h \
    objects/metiersentity.h \
    nodepenalty.h \
    dialogs/graphexportproperties.h \
    objects/windmillentity.h \
    modelobjects/windmilldata.h \
    mapobjects/windmillobject.h \
    graphbuilder_shp.h \
    objects/firmentity.h \
    modelobjects/firmdata.h \
    mapobjects/firmmapobject.h \
    utils/interestinglist.h \
    objects/bfunctionalgroupsentity.h \
    utils/interestinglistwithspecialvalues.h \
    plots/benthosstatsplot.h \
    stats/benthosstats.h \
    stats/fishfarmsstats.h \
    plots/fishfarmsstatsplot.h \
    objects/ffarmtypesentity.h \
    stats/shipsstats.h \
    stats/windfarmsstats.h \
    plots/windfarmsstatsplot.h \
    plots/shipsstatsplot.h \
    objects/wfarmtypesentity.h \
    objects/shiptypesentity.h \
    plots/nationsstatsplot.h \
    plots/populationsstatplot.h \
    plots/harboursstatplot.h \
    plots/metiersstatsplot.h \
    stats/statsutils.h \
    mapsdataprovider.h \
    mapsdatastructures.h \
    simplenoninterestingnodesgraphsimplifier.h \
    plots/BaseStatsPlotImpl.h \
    algo/AStarShortestPath.h \
    algo/DijkstraShortestPath.h \
    algo/ShortestPathAlgorithm.h

### mSQLiteCpp dependency

#DEFINES += BUILD_MSQLITECPP
#INCLUDEPATH += $$SRCROOT/mSqliteCpp/include
#SOURCES += $$SRCROOT/mSqliteCpp/src/*.cpp
#HEADERS += $$SRCROOT/mSqliteCpp/include/*.h
#!win32: LIBS += -lsqlite3

LIBS += -lmsqlitecpp -lsqlite3


#win32 {
#    DEFINES += EMBED_MSQLITECPP
#    SOURCES += ../commons/storage/sqlite3.c
#}

### End mSqliteCpp Depedency

RESOURCES += \
    qtgui.qrc

OTHER_FILES += \
    qtgui.rc \
    ../docs/output_fileformats.txt \
    palettes/heat_colors_12.p2c \
    ../docs/database.txt \
    ../docs/dbstruct.sh \
    palettes/pop_colors.p2c \
    ../docs/input_fileformats.txt \
    ../Doxyfile \
    palettes/tariffs_colors.p2c \
    palettes/benthos_colors.p2c \
    palettes/bathy_fishfarms.p2c \
    palettes/bathy_colors.p2c \
    palettes/nitrogen_colors.p2c \
    palettes/phosphorus_colors.p2c \
    palettes/oxygen_colors.p2c \
    palettes/dissolvedcarbon_colors.p2c \
    palettes/salinity_colors.p2c \
    palettes/sst_colors.p2c \
    palettes/wind_colors.p2c \
    extra/displace.desktop \
    icons/displace.icns \
    resources/Info.plist

TRANSLATIONS += \
    translations/displace_it.ts \
    translations/displace_fr.ts \
    translations/displace_de.ts


target.path=$${PREFIX}/bin

extra.path=$${PREFIX}/share/applications
extra.files=extra/displace.desktop

icons.path=$${PREFIX}/share/pixmaps
icons.files=icons/displace.png

INSTALLS += target icons extra


