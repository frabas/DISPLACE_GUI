
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CPACK_PACKAGE_NAME "Displace")
set(CPACK_PACKAGE_VENDOR "DTU Aqua - Technical University of Denmark")

set(CPACK_PACKAGE_VERSION ${displace_VERSION})
set(CPACK_SOURCE_IGNORE_FILES "cmake-build*;build;\\\\.git;\\\\.idea")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

set(CPACK_COMPONENTS_ALL gui simulator libraries tools tests)

set(CPACK_COMPONENT_gui_DISPLAY_NAME "Displace Graphical User Interface")
set(CPACK_COMPONENT_simulator_DISPLAY_NAME "Displace Command Line Simulator")
set(CPACK_COMPONENT_libraries_DISPLAY_NAME "Displace Common Libraries")
set(CPACK_COMPONENT_tools_DISPLAY_NAME "Displace Additional Tools")
set(CPACK_COMPONENT_tests_DISPLAY_NAME "Unit and regression tests")

set(CPACK_COMPONENT_gui_DEPENDS libraries)
set(CPACK_COMPONENT_simulator_DEPENDS libraries)
set(CPACK_COMPONENT_tools_DEPENDS libraries)
set(CPACK_COMPONENT_tests_DEPENDS libraries)

if (APPLE)
    set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/qtgui/resources/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/qtgui/icons/displace.icns")
endif (APPLE)


if (APPLE)
    set(CPACK_BUNDLE_NAME "DisplaceProject")

    # Note Mac specific extension .app
set(APPS "\${CMAKE_INSTALL_PREFIX}/${CPACK_BUNDLE_NAME}.app")

    # Directories to look for dependencies
    set(DIRS ${CMAKE_BINARY_DIR})

    install(CODE "include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")

    set(CPACK_GENERATOR "DRAGNDROP")
endif (APPLE)

include(CPack)

