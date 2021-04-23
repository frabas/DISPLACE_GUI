TEMPLATE= lib
TARGET=displacecommons
DESTDIR=../
CONFIG += c++14 dll


include ("$$top_srcdir/localconfig.pri")
macx:DESTDIR=$$LIBDESTDIR

GEOGRAPHICLIB=Geographic

win32 {
    CONFIG(debug,release|debug){
        GEOGRAPHICLIB=Geographic_d
    }
}

DEFINES += COMMONS_LIBRARY WITH_BOOST
INCLUDEPATH +=../include/ ../formats . ../sparsepp
LIBS += -L.. -lformats -l$${GEOGRAPHICLIB}

win32 {
    # No crash handler support for Windows
    DEFINES += NO_CRASHHANDLER
    DEFINES += EMBED_MSQLITECPP

    # For GetProcessMemoryInfo()
    LIBS += -lpsapi
}

unix:!macx {
    LIBS += -lrt
}

macx {
    # No crash handler support for MacOS
    DEFINES += NO_CRASHHANDLER
}

message("Link Path: $$LIBS")

SOURCES= \
    readdata.cpp \
    myutils.cpp \
    Population.cpp \
    Fishfarm.cpp \
    Windmill.cpp \
    Node.cpp \
    Tac.cpp \
    Benthos.cpp \
    Harbour.cpp \
    Ship.cpp \
    Vessel.cpp \
    Metier.cpp \
    Firm.cpp \
    myRutils.cpp \
    mkpath.cpp \
    memoryinfo.cpp \
    options.cpp \
    statics.cpp \
    simulation.cpp \
    ConfigTables.cpp \
    SimModel.cpp \
    dtree/decisiontree.cpp \
    dtree/variables.cpp \
    dtree/dtnode.cpp \
    dtree/decisiontreemanager.cpp \
    dtree/externalstatemanager.cpp \
    tseries/timeseries.cpp \
    tseries/timeseriesmanager.cpp \
    utils/CrashHandler.cpp \
    pathshop.cpp \
    storage/tables/vesseldeftable.cpp \
    storage/sqliteoutputstorage.cpp \
    storage/tables/vesselslogliketable.cpp \
    storage/tables/poptable.cpp \
    storage/tables/vesselvmsliketable.cpp \
    storage/tables/fishfarmstable.cpp \
    storage/tables/fishfarmsdeftable.cpp \
    storage/tables/popquotastable.cpp \
    storage/tables/windfarmstable.cpp \
    storage/tables/vesselsloglikecatchestable.cpp \
    storage/tables/nodesdeftable.cpp \
    storage/tables/popdyntable.cpp \
    storage/tables/metadatatable.cpp \
    storage/modelmetadataaccessor.cpp \
    storage/tables/vesselvmslikefpingsonlytable.cpp \
    storage/tables/funcgroupstable.cpp \
    storage/tables/shipstable.cpp \
    diffusion.cpp \
    storage/tables/nodesenvttable.cpp \
    storage/tables/nodesstattable.cpp \
    storage/tables/nodestariffstattable.cpp \
    shortestpath/AStarShortestPathFinder.cpp \
    shortestpath/GeoGraph.cpp \
    shortestpath/GeoGraphLoader.cpp \
    ModelLoader.cpp TextfileModelLoader.cpp \
    dataloaderbenthos.cpp dataloadercommercialships.cpp dataloaderfishfarms.cpp \
    dataloadermetiers.cpp dataloaderpops.cpp dataloadervessels.cpp dataloaderwindmills.cpp

HEADERS= \
    ../include/readdata.h \
    ../include/myutils.h \
    ../include/Population.h \
    ../include/Fishfarm.h \
    ../include/Windmill.h \
    ../include/Node.h \
    ../include/Tac.h \
    ../include/Benthos.h \
    ../include/Harbour.h \
    ../include/Ship.h \
    ../include/Vessel.h \
    ../include/Metier.h \
    ../include/Firm.h \
    ../include/myRutils.h \
    ../include/mkpath.h \
    ../include/m_constants.h \
    ../include/memoryinfo.h \
    ../include/helpers.h \
    ../include/options.h \
    ../include/guiproto_struct.h \
    ../include/version.h \
    ../include/utils/make_unique.h \
    ../include/vesselcalendar.h \
    ../include/idtypes.h \
    ../include/idtypeshelpers.h \
    ../include/ConfigTable.h \
    simulation.h \
    SimModel.h \
    dtree/decisiontree.h \
    dtree/variables.h \
    dtree/dtnode.h \
    dtree/dtnodeextra.h \
    dtree/decisiontreemanager.h \
    dtree/stateevaluator.h \
    dtree/externalstatemanager.h \
    dtree/commonstateevaluators.h \
    ../include/comstructs.h \
    tseries/timeseries.h \
    tseries/timeseriesmanager.h \
    dtree/evaluators/timeseriesevaluator.h \
    utils/CrashHandler.h \
    dtree/vesselsevaluators.h \
    commons_global.h \
    pathshop.h \
    storage/tables/vesseldeftable.h \
    storage/sqliteoutputstorage.h \
    storage/tables/vesselslogliketable.h \
    storage/tables/poptable.h \
    ../mSqliteCpp/include/msqlitecpp.h \
    storage/tables/vesselvmsliketable.h \
    storage/tables/fishfarmstable.h \
    storage/tables/fishfarmsdeftable.h \
    storage/tables/popquotastable.h \
    storage/tables/windfarmstable.h \
    storage/tables/vesselsloglikecatchestable.h \
    storage/tables/nodesdeftable.h \
    storage/tables/popdyntable.h \
    ../mSqliteCpp/include/msqlitecpp.h \
    ../mSqliteCpp/include/sqlitefielddef.h \
    ../mSqliteCpp/include/sqlitefieldsop.h \
    ../mSqliteCpp/include/sqlitestatement.h \
    ../mSqliteCpp/include/sqlitestatementformatters.h \
    ../mSqliteCpp/include/sqlitestorage.h \
    ../mSqliteCpp/include/sqlitetable.h \
    ../mSqliteCpp/include/sqlitetransaction.h \
    storage/tables/metadatatable.h \
    storage/modelconfig.h \
    storage/modelmetadataaccessor.h \
    storage/tables/vesselvmslikefpingsonlytable.h \
    storage/tables/funcgroupstable.h \
    storage/tables/shipstable.h \
    ../include/diffusion.h \
    storage/tables/nodesenvttable.h \
    storage/tables/nodesstattable.h \
    storage/tables/nodestariffstattable.h \
    ../include/StrongType.h \
    storage/cacheddatastorage.h \
    shortestpath/AStarShortestPathFinder.h \
    shortestpath/GeoGraphLoader.h \
    shortestpath/GeoGraph.h \
    ModelLoader.h  TextfileModelLoader.h \
    dataloader.h dataloaderbenthos.h dataloadercommercialships.h \
    dataloaderfishfarms.h dataloadermetiers.h dataloaderpops.h dataloadervessels.h dataloaderwindmills.h



### mSQLiteCpp dependency

!win32 {
    DEFINES += BUILD_MSQLITECPP
}

#INCLUDEPATH += $$top_srcdir/mSqliteCpp/include
#SOURCES += $$top_srcdir/mSqliteCpp/src/*.cpp
#HEADERS += $$top_srcdir/mSqliteCpp/include/*.h

LIBS += -lmsqlitecpp -lsqlite3

#win32 {
#    SOURCES += storage/sqlite3.c
#}

### End mSqliteCpp Depedency

## Do not add this if you don't support IPC queues

!no_ipc {
    SOURCES += \
        ipcqueue.cpp

    HEADERS += \
        ../include/ipcqueue.h \
        ../include/ipcmsgtypes.h \
}

OTHER_FILES += \
    Makefile.hpc

target.path = $${PREFIX}/lib
headers.path = $${PREFIX}/include/
headers.files = $${HEADERS}

INSTALLS += target
