find_file(Sparsepp_HEADER_FILE
        NAMES spp.h
        PATHS $ENV{SPARSEPP_ROOT} ${SPARSEPP_ROOT} ${CMAKE_SOURCE_DIR}/sparsepp
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
        message(STATUS "Sparsepp not found, cloning from GitHub...")
    endif (NOT Sparsepp_FIND_QUIETLY)

    # Clone sparsepp repository if not found
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/greg7mdp/sparsepp.git ${CMAKE_SOURCE_DIR}/sparsepp
            RESULT_VARIABLE GIT_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )

        if(GIT_RESULT EQUAL 0)
            message(STATUS "Sparsepp cloned successfully to ${CMAKE_SOURCE_DIR}/sparsepp")
            set(Sparsepp_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/sparsepp")
            set(Sparsepp_FOUND yes)
        else()
            if (Sparsepp_FIND_REQUIRED)
                message(FATAL_ERROR "Failed to clone sparsepp repository.")
            endif()
        endif()
    else()
        if (Sparsepp_FIND_REQUIRED)
            message(FATAL_ERROR "Git not found, cannot clone sparsepp. Try setting SPARSEPP_ROOT environment variable.")
        endif()
    endif()
endif ()

if (Sparsepp_FOUND)
    message("Sparsepp Headers: ${Sparsepp_INCLUDE_DIRS}")
endif ()
