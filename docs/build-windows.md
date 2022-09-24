# Building on Windows 

## Requirements

To build on Windows you need to install vcpkg.

### Setting up vcpkg

```
$ git clone https://www.github.com/Microsoft/vcpkg.git
$ cd vcpkg
$ .\bootstrap-vcpkg.bat

(optional)
$ vcpkg.exe integrate install

$ .\vcpkg.exe --triplet x64-windows install <list-of-packages-to-install>
```

### Setting up the default triplet

Set default triplet to 64 bit windows.
Using Powershell:

```
PS ...> $env:VCPKG_DEFAULT_TRIPLET = "x64-Windows"
```

As an alternative, just add the option: `--triplet x46-windows` to every command.

## Install the required dependencies

This is a list of the packages that must be explicitly installed from vcpkg:

- GeographicLib 
- cgal 
- gdal 
- sparsepp 
- boost-date-time 
- boost-filesystem 
- boost-system 
- boost-thread 
- boost-program-options 
- boost-log 
- boost-test
- msqlitecpp

msqlitecpp is not provided with vcpkg, so you need to copy the vcpkg/msqlitecpp from the msqlitecpp source tree directory in the ports directory of the vcpkg source tree.

So for each package, build and install it:

```
PS ...> .\vcpkg.exe install GeographicLib
```

and so on for all packages.

We provide the vcpkg-setup.bat file. Simply copy it into the root of the vcpkg directory, and run it:

```
PS ...> .\vcpkg-setup.bat build
```


## Export the SDK package

To export the whole sdk, you must run with the export option, specifying the full list of dependencies.

```
$ .\vcpkg.exe export --7zip GeographicLib cgal gdal ....
```

as an alternative, simply run our script with the export command:

```
PS ...> .\vcpkg-setup.bat export
```

## Building (Visual Studio)

On Visual Studio, you need to add the following options to the CMake settings.

Select "Cmake Settings for displace" in the Project menu.

For each configuration (debug, release, etc) add the following line in the "Cmake command argument" box:



```
-DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64 -DWITH_TESTS=OFF -DCMAKE_TOOLCHAIN_FILE=C:/DISPLACE-vcpkg/scripts/buildsystems/vcpkg.cmake
```

Then build the project as usual.


## Building (command line)

To build displace with VCPKG, the procedure is similar as building on other OSs.

First, run cmake with the proper options:

```
cmake -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64 -DWITH_TESTS=OFF -DCMAKE_TOOLCHAIN_FILE=C:/DISPLACE-vcpkg/scripts/buildsystems/vcpkg.cmake
```

we assume vcpkg is installed in C:\Displace-vcpkg, if this is not the case, correct the command above with the proper
path.

Then build with

```

```
