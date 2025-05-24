# Common Ambient Variables:
#   CURRENT_BUILDTREES_DIR    = ${VCPKG_ROOT_DIR}\buildtrees\${PORT}
#   CURRENT_PACKAGES_DIR      = ${VCPKG_ROOT_DIR}\packages\${PORT}_${TARGET_TRIPLET}
#   CURRENT_PORT_DIR          = ${VCPKG_ROOT_DIR}\ports\${PORT}
#   PORT                      = current port name (zlib, etc)
#   TARGET_TRIPLET            = current triplet (x86-windows, x64-windows-static, etc)
#   VCPKG_CRT_LINKAGE         = C runtime linkage type (static, dynamic)
#   VCPKG_LIBRARY_LINKAGE     = target library linkage type (static, dynamic)
#   VCPKG_ROOT_DIR            = <C:\path\to\current\vcpkg>
#   VCPKG_TARGET_ARCHITECTURE = target architecture (x64, x86, arm)
#

include(vcpkg_common_functions)

vcpkg_from_github(
		OUT_SOURCE_PATH SOURCE_PATH
		REPO studiofuga/msqlitecpp
		REF "v${VERSION}"
		SHA512 e33d1caac30e80b9d0ead5e2213c5b1ce713f8690bdb858ee4dede1fbe1f70c02317f19e6d1e12b17939150dbebe78ccda3083c6e968e896e66fda9a7d52194a  # This is a temporary value. We will modify this value in the next section.
		HEAD_REF master
)

vcpkg_cmake_configure(
		SOURCE_PATH "${SOURCE_PATH}"
		PREFER_NINJA # Disable this option if project cannot be built with Ninja
		OPTIONS -DENABLE_TEST=OFF -DENABLE_PROFILER=OFF -DENABLE_SQLITE_AMALGAMATION=OFF -DENABLE_CODE_COVERAGE=OFF
)

vcpkg_install_cmake()

# Remove the EXTRA include files

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

# move the LIB files to the correct location

file(COPY
        ${CURRENT_PACKAGES_DIR}/debug/share/msqlitecpp/msqlitecppTargets-debug.cmake
        DESTINATION
        ${CURRENT_PACKAGES_DIR}/share/msqlitecpp)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)

# Handle copyright

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/msqlitecpp RENAME copyright)

# Post-build test for cmake libraries
# vcpkg_test_cmake(PACKAGE_NAME msqlitecpp)
