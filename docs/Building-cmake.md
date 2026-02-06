# Building displace with CMake

## What is CMake?

"[CMake](https://cmake.org) is an open-source, cross-platform family of tools designed to build, test and package software"

## Building

To build, simply create an out-of-tree folder, and launch cmake spacifying the path of the main CMakeLists.txt file:

### Linux

```bash
$ cd displace
$ mkdir build
$ cd build
$ cmake ..
```

### Windows

You can use the cmake GUI to select the building directory and the root of the project.

## Building with Out-of-system-tree libraries

If the dependent libraries are installed out of the system tree (for example, because your distribution don't provide
a proper package, or you want to compile with a different version, or in Windows), you may instruct CMake to search 
for the library in the correct directory.

For GeographicLib, use `-DGeographicLib_DIR=`



```
cmake 
    -DGeographicLib_DIR=/opt/GeographicLib-1.49/install/usr/local/lib/cmake/GeographicLib 
```

### Specifying QT paths

If cmake can't find the right path to a package, or you want to select a different version of the package, you can 
pass some variable on the command line.

This happens with Qt when they are installed in a non-system path, on Windows and on linux when you install a non 
packaged version of the library.

For Qt, pass the `CMAKE_PREFIX_PATH` variable with the path to the `lib/cmake` folder of your installation.
If Qt has been installed in `/opt/Qt/5.10.1`, for example, specify:

```bash
$ cmake -DCMAKE_PREFIX_PATH=/opt/Qt/5.10.1/gcc_64/lib/cmake/
```

In Windows, from the GUI, create a variable in the cache and put the path (see next section).


### Windows compilation: inform the IDE for where to find CMake and the DISPLACE SDK

First install Qt 5.15.5 https://www.qt.io/download and QtCreator that comes with.

Alternatively, install JetBrains CLion, but the smooth edition of ui files will require QtCreator anyway.

But remember to set et up CLion in Menu>File> settings
For amd64 architecture because we are working in 64bits.

![alt text](./images/set_CLion_for_amd64.png)

Install CMake on your computer.

Unzip the Displace sdk e.g. vcpkg-export-20181211-212458.7z (build with https://docs.microsoft.com/en-us/cpp/vcpkg?view=vs-2017, 
see Building.md, you will found the sdk on the DISPLACE_release\WindowsSupport on
 google drive https://drive.google.com/drive/folders/0B7xTkBu0-QI5bnIxS3c4dy0tSUU ) imperatively in C:\\ and rename 
the folder for "vcpkg"

 Open the DISPLACE project e.g. in QtCreator or CLion with the CMakeList.txt file

 Configure CMake by adding a arg variable, for example:
```bash
CMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Also erase in the Release output folder or the cmake CMakeCache.txt and the subfolder CMakeFiles in the Release folder
if something goes wrong.

In QtCreator:

![alt text](./images/set_CMAKE_TOOLCHAIN_FILE_in_QtCreator.png)

In CLion:

![alt text](./images/set_CMAKE_TOOLCHAIN_FILE_in_CLion.png)

Clear or reset cache and re-run CMake

Compile DISPLACE_GUI

Set the Run on displacegui and run!

(If CLion, Run displace first to Build it with CMake)

on Windows, possibly check PATH for e.g.  `C:\Qt\5.12.0\msvc2017_64\bin` that should be in `PATH`




