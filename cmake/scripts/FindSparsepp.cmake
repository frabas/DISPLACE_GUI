find_file(Sparsepp_HEADER_FILE
        NAMES spp.h
        PATHS $ENV{SPARSEPP_ROOT} ${SPARSEPP_ROOT}
        PATH_SUFFIXES sparsepp
        )
mark_as_advanced(Sparsepp_HEADER_FILE)

get_filename_component(Sparsepp_INCLUDE_DIRS "${Sparsepp_HEADER_FILE}" PATH)
get_filename_component(Sparsepp_INCLUDE_DIRS "${Sparsepp_INCLUDE_DIRS}" PATH)

if (Sparsepp_HEADER_FILE)
    set(Sparsepp_FOUND yes)
endif ()

if (NOT Sparsepp_FOUND)
    if (NOT Sparsepp_FIND_QUIETLY)
        message(STATUS "Sparsepp not found, try setting SPARSEPP_ROOT environment variable.")
    endif (NOT Sparsepp_FIND_QUIETLY)
    if (Sparsepp_FIND_REQUIRED)
        message(FATAL_ERROR "Sparsepp not found, bur it's required.")
    endif (Sparsepp_FIND_REQUIRED)
else()
    message("Sparsepp Headers: ${Sparsepp_INCLUDE_DIRS}")
endif (NOT Sparsepp_FOUND)
