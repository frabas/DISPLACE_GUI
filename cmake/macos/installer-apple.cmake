set(APP_NAME displacegui)

# Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
#install(TARGETS ${APP_NAME}
#        BUNDLE DESTINATION . COMPONENT Runtime
#        RUNTIME DESTINATION bin COMPONENT Runtime
#        )

# Note Mac specific extension .app
set(APPS "\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app")

# Directories to look for dependencies
set(DIRS ${CMAKE_BINARY_DIR}/qtdir)
install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")

set(CPACK_GENERATOR "DragNDrop")
include(CPack)