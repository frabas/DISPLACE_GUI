# Building on Windows 

## Requirements

To build on Windows you need to install vcpkg.

Since 1.3.2, we added a custom overlay to build the dependencies that are not in the official vcpkg distribution.

They can be found in the `vcpkg-overlay` subdirectory.


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

## Build

Vcpkg has been integrated into the cmake workflow by adding a custom profile. In the root of this
project there are two files: `CMakePresets.json` and `CMakeUserPresets.json.sample`.

The latter is just a template, and it should be copied on `CMakeUserPresets.json` in case user want to adjust the
settings. Since the content is host-specific, it should never be committed into the repository.

Copy the .sample file into .json, and adjust it. In particular, fix the `"VCPKG_ROOT": "C:/vcpkg"` line with the
path to the vcpkg directory that fits your host.

After that, before configuring cmake, select the profile named `default` or `vcpkg` if the version provided by 
the project is ok.

```
cmake --preset=default
```

Then configure and build the project as normal. 


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
