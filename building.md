* Using vcpkg 


** Install the required dependencies

This is a list of the packages that must be explicitly installed from vcpkg:
    GeographicLib
    cgal
    gdal
    msqlitecpp
    boost-program-options
    boost-interprocess
	qt5-base (optional)

** Setting up vcpkg
	
$ git clone https://www.github.com/Microsoft/vcpkg.git
$ cd vcpkg
$ .\bootstrap-vcpkg.bat

(optional)
$ vcpkg.exe integrate install

$ .\vcpkg.exe --triplet x64-windows install <list-of-packages-to-install>

** Building the SDK package

$ .\vcpkg.exe export --7zip --triplet x64-windows <list of packages to export>

