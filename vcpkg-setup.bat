# vcpkg-setup script
# Version: 2020.06.01

echo off

set MODULES=GeographicLib cgal gdal sparsepp boost-date-time boost-filesystem boost-system boost-thread boost-program-options boost-log boost-test msqlitecpp
set TRIPLET=x64-windows
set EXE=%~0

SET CMD=%~1

IF "%CMD%" == "" GOTO help
IF "%CMD%" == "build" goto build
IF "%CMD%" == "export" goto export

goto help

:build
#vcpkg.exe --triplet %TRIPLET% build %MODULES%
vcpkg.exe --recurse --triplet %TRIPLET% install %MODULES%

goto end

:export
vcpkg.exe --triplet %TRIPLET% export --7zip %MODULES%

goto end

:help
echo "Usage: %EXE% [build|export]"

:end
