project(formats)

add_definitions(-DFORMATS_LIBRARY)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
)

set(SOURCES
        legacy/ConfigFileReader.cpp legacy/ConfigFileReader.h
        utils/KeyValueReader.cpp utils/KeyValueReader.h
        utils/LineNumberReader.cpp utils/LineNumberReader.h
        utils/prettyprint.h
        legacy/NodesFileReader.h utils/MultifieldReader.h
        )

add_library(formats SHARED
        ${SOURCES}
        )

# Set target directory
install(TARGETS formats LIBRARY DESTINATION lib COMPONENT libraries)

#add_subdirectory(test)
