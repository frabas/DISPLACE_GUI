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

In Windows, from the GUI, create a variable in the cache and put the path.

