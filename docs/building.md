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