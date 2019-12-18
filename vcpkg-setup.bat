#echo off

set MODULES=GeographicLib cgal gdal msqlitecpp boost-program-options boost-interprocess boost-geometry sparsepp boost-log boost-filesystem
set TRIPLET=x64-windows
set EXE=%~0

SET CMD=%~1

IF "%CMD%" == "" GOTO help
IF "%CMD%" == "build" goto build
IF "%CMD%" == "export" goto export

goto help

:build
#vcpkg.exe --triplet %TRIPLET% build %MODULES%
vcpkg.exe --triplet %TRIPLET% install %MODULES%

goto end

:export
vcpkg.exe --triplet %TRIPLET% export --7zip %MODULES%

goto end

:help
echo "Usage: %EXE% [build|export]"

:end
