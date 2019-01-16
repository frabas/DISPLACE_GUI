= Using vcpkg 

== Setting up vcpkg
	
$ git clone https://www.github.com/Microsoft/vcpkg.git
$ cd vcpkg
$ .\bootstrap-vcpkg.bat

(optional)
$ vcpkg.exe integrate install

$ .\vcpkg.exe --triplet x64-windows install <list-of-packages-to-install>


== Setting up the default triplet

Set default triplet to 64 bit windows.
Using Powershell: 

PS ...> $env:VCPKG_DEFAULT_TRIPLET = "x64-Windows"

== Install the required dependencies

This is a list of the packages that must be explicitly installed from vcpkg:
    *GeographicLib
    *cgal
    *gdal
    *msqlitecpp
    *boost-program-options
    *boost-interprocess
	*sparsepp
	qt5-base (optional)

So for each package, build and install it:

PS ...> .\vcpkg.exe install GeographicLib

and so on for all packages.

== Export the SDK package

$ .\vcpkg.exe export --7zip GeographicLib cgal gdal msqlitecpp boost-program-options boost-interprocess sparsepp


== Setting up cmake

Cmake requires the following variables:

CMAKE_TOOLCHAIN_FILE=<path-to-sdk>\scripts\buildsystems\vcpkg.cmake

You can add the following lines to CMakeSettins.json to setup visual studio


	"variables": [
        {
          "name": "CMAKE_TOOLCHAIN_FILE",
          "value": "C:\\path-to-sdk\\scripts\\buildsystems\\vcpkg.cmake"
        },
        {
          "name": "CMAKE_PREFIX_PATH",
          "value": "C:\\Qt\\5.12.0\\msvc2017_64"
        }
      ]
	  
	  
after each "ctestCommandsArgs" key.
