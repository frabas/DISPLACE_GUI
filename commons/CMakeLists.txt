
add_definitions(-DCOMMONS_LIBRARY -DWITH_BOOST)

if (WIN32)
    add_definitions(
            -DNO_CRASHHANDLER
            -DEMBED_MSQLITECPP
    )
endif (WIN32)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/mSqliteCpp/include
        ${CMAKE_SOURCE_DIR}/include/
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/sparsepp
)

if(GEOGRAPHIC_INCLUDE)
    message("GeographicLib Include: ${GEOGRAPHIC_INCLUDE}")
    include_directories(${GEOGRAPHIC_INCLUDE})
endif()

if (GEOGRAPHIC_LIBDIR)
    message("GeographicLib Lib: ${GEOGRAPHIC_LIBDIR}")
    link_directories(${GEOGRAPHIC_LIBDIR})
endif()

set(SOURCES
        commons_global.h
        diffusion.cpp
        readdata.cpp
        myutils.cpp
        Population.cpp
        Fishfarm.cpp
        Windmill.cpp
        Node.cpp
        Tac.cpp
        Benthos.cpp
        Harbour.cpp
        Ship.cpp
        Vessel.cpp
        Metier.cpp
        Firm.cpp
        myRutils.cpp
        mkpath.cpp
        memoryinfo.cpp
        options.cpp
        statics.cpp
        simulation.cpp
        ipcqueue.cpp
        dtree/decisiontree.cpp
        dtree/variables.cpp
        dtree/dtnode.cpp
        dtree/decisiontreemanager.cpp
        dtree/externalstatemanager.cpp
        tseries/timeseries.cpp
        tseries/timeseriesmanager.cpp
        pathshop.cpp
        storage/tables/vesseldeftable.cpp
        storage/sqliteoutputstorage.cpp
        storage/tables/vesselslogliketable.cpp
        storage/tables/poptable.cpp
        storage/tables/vesselvmsliketable.cpp
        storage/tables/fishfarmstable.cpp
        storage/tables/windfarmstable.cpp
        storage/tables/vesselsloglikecatchestable.cpp
        storage/tables/nodesdeftable.cpp
        storage/tables/popdyntable.cpp
        storage/tables/popquotastable.cpp
        storage/tables/metadatatable.cpp
        storage/modelmetadataaccessor.cpp
        storage/tables/vesselvmslikefpingsonlytable.cpp
        storage/tables/funcgroupstable.cpp
        storage/tables/shipstable.cpp
        diffusion.cpp
        storage/tables/nodesenvttable.cpp
        storage/tables/nodesstattable.cpp
        storage/tables/nodestariffstattable.cpp
        ../include/readdata.h
        ../include/myutils.h
        ../include/Population.h
        ../include/Fishfarm.h
        ../include/Windmill.h
        ../include/Node.h
        ../include/Tac.h
        ../include/Benthos.h
        ../include/Harbour.h
        ../include/Ship.h
        ../include/Vessel.h
        ../include/Metier.h
        ../include/Firm.h
        ../include/myRutils.h
        ../include/mkpath.h
        ../include/m_constants.h
        ../include/memoryinfo.h
        ../include/helpers.h
        ../include/options.h
        ../include/guiproto_struct.h
        ../include/version.h
        ../include/utils/make_unique.h
        ../include/vesselcalendar.h
        ../include/idtypes.h
        ../include/idtypeshelpers.h
        simulation.h
        dtree/decisiontree.h
        dtree/variables.h
        dtree/dtnode.h
        dtree/dtnodeextra.h
        dtree/decisiontreemanager.h
        dtree/stateevaluator.h
        dtree/externalstatemanager.h
        dtree/commonstateevaluators.h
        ../include/comstructs.h
        tseries/timeseries.h
        tseries/timeseriesmanager.h
        dtree/evaluators/timeseriesevaluator.h
        dtree/vesselsevaluators.h
        commons_global.h
        pathshop.h
        storage/tables/vesseldeftable.h
        storage/sqliteoutputstorage.h
        storage/tables/vesselslogliketable.h
        storage/tables/poptable.h
        ../mSqliteCpp/include/msqlitecpp.h
        storage/tables/vesselvmsliketable.h
        storage/tables/fishfarmstable.h
        storage/tables/windfarmstable.h
        storage/tables/vesselsloglikecatchestable.h
        storage/tables/nodesdeftable.h
        storage/tables/popdyntable.h
        storage/tables/popquotastable.h
        ../mSqliteCpp/include/msqlitecpp.h
        ../mSqliteCpp/include/sqlitefielddef.h
        ../mSqliteCpp/include/sqlitefieldsop.h
        ../mSqliteCpp/include/sqlitestatement.h
        ../mSqliteCpp/include/sqlitestatementformatters.h
        ../mSqliteCpp/include/sqlitestorage.h
        ../mSqliteCpp/include/sqlitetable.h
        ../mSqliteCpp/include/sqlitetransaction.h
        storage/tables/metadatatable.h
        storage/modelconfig.h
        storage/modelmetadataaccessor.h
        storage/tables/vesselvmslikefpingsonlytable.h
        storage/tables/funcgroupstable.h
        storage/tables/shipstable.h
        ../include/diffusion.h
        storage/tables/nodesenvttable.h
        storage/tables/nodesstattable.h
        storage/tables/nodestariffstattable.h
        ../include/StrongType.h
        ../include/ipcqueue.h
        ../include/ipcmsgtypes.h
        utils/CrashHandler.cpp
        storage/tables/fishfarmsdeftable.cpp storage/tables/fishfarmsdeftable.h
        shortestpath/AStarShortestPathFinder.h shortestpath/AStarShortestPathFinder.cpp
        shortestpath/GeoGraph.h shortestpath/GeoGraph.cpp
        shortestpath/GeoGraphLoader.h shortestpath/GeoGraphLoader.cpp
        )

if (WIN32 OR APPLE)
    add_definitions(-DNO_CRASHHANDLER)
endif (WIN32 OR APPLE)


# Add msqlite
file(GLOB MSQLITE_SOURCES ${CMAKE_SOURCE_DIR}/mSqliteCpp/src/*.cpp)
file(GLOB MSQLITE_HEADERS ${CMAKE_SOURCE_DIR}/mSqliteCpp/include/*.h)


if (NOT WIN32)
    list(APPEND LIBRARIES sqlite3)
endif (NOT WIN32)

if (WIN32)
    list(APPEND LIBRARIES psapi)
    list(APPEND SOURCES
            storage/sqlite3.c
            )
    include_directories( ${CMAKE_SOURCE_DIR}/commons/storage)
endif (WIN32)

if (NOT APPLE AND NOT WIN32)
    list(APPEND LIBRARIES rt)
endif (NOT APPLE AND NOT WIN32)

add_library(commons SHARED
        ${SOURCES}
        ${MSQLITE_SOURCES} ${MSQLITE_HEADERS}
        )

target_link_libraries(commons
        formats
        Geographic
        ${LIBRARIES}
        )

# Set target directory
install(TARGETS commons DESTINATION lib)
