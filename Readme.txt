README
======

Summary
- How to build
- GDAL notes


How to build
============


Windows
-------

* Prerequisites:

Since version 0.5.0 Displace requires GDAL library.
Since version 0.5.4, Displace supports builds under 64bit MinGW.
Since version 0.6.0, Displace requires the Boost (Graph) Library.
Since version 0.6.1, Displace depends on GeographicLib (http://geographiclib.sourceforge.net/)

You can find the instructions for GDAL installation and 64bit mingw in a section below.


* Building and installing prerequisite libraries

Boost:
Boost (1.57.0 at this time) are installed in c:\mingw\boost-1_57_0
To install, just unpack the zip file in c:\mingw. 
If this is not the case, or a later version is used, change the INCLUDEPATH variable in qtgui/qtgui.pro

Ming64 and gdal: 

NOTE: Compiling the project under 64bit compilers is HIGLY RECOMMENDED. 
Displace (both Gui and Simulator) requires huge amounts of data that 32bit Windows can't handle properly.
The 32 bit Windows subsystem has a limit of 1Gb allocated memory, when the total size of allocate data 
exceed this limit, the program will crash. 
So Displace requires compilation under 64bit systems.

Unfortunately at this time, neither QT and GDAL are available in 64bit binary form from the official channels,
so some hand work is required.

0) We assume that you have MSYS and QT with MinGW 32 bit already installed. Please refer to the proper websites for instructions.
You'll have also Qt Creator already installed.

1) Installation of 64bit QT5.3.1

Go to http://www.tver-soft.org/qt64 and download the proper installer (MinGW 64bit, OpenGL, seh Exception Handling, Posix Threading)

At this time, this is the direct link.

http://sourceforge.net/projects/qtx64/files/qt-x64/5.3.2/mingw-4.9/seh/qt-5.3.2-x64-mingw491r1-seh-opengl.exe/download

Install the packet. You don't need QT sources or examples, but you need compilers (they are included). I suggest to use the 
default qt path, so you'll have all qt related libraries (and mingw) under the same path: C:\Qt\qt-5.3.2-x64-mingw491r1-seh-opengl

2) Compile and install zlib

GDAL requires zlib to be a shared library, but unfortunately the previous package only provide it in form of a static library.
This way, only static gdal library will be compiled, and you'll be unable to link displace and the gdal utilities will bloat.

So download the zlib sources from http://www.zlib.net/, unpack it somewhere (I suggest some path under the mingw path) and compile
To be able to use the 64bit compiler from the default MSYS environment, you'll need to prepend the compiler path to the system PATH

$ export PATH=/c/qt-5.3.2-x64-mingw491r1-seh-opengl/mingw64/bin:$PATH

edit the win32\Makefile.gcc file this way:

change the PREFIX variable

PREFIX = x86_64-w64-mingw32-


Modify the following variables (search and correct them accordingly) :

AR = ar
RC = windres

STRIP = strip


then compile the library and install:

$ make -f win32/Makefile.gcc
...
$ cp zlib1.dll /usr/local/bin
$ cp zconf.h zlib.h /usr/local/include
$ cp libz.a /usr/local/lib
$ cp libz.dll.a /usr/local/lib/libz.dll.a

Change /usr/local/ accordingly if needed.

3) Compile GDAL

Enter the gdal source (they can be downloaded from: http://trac.osgeo.org/gdal/wiki/DownloadSource )
Use configure & make, as usual: 

$ ./configure  --host=x86_64-w64-mingw32 --disable-static --enable-shared

FIX the compilation script: 
compiling under MinGW 64 requires link gdal with iconv: so open the GDALmake.opt file and change the following line:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL)

to:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL) -liconv

Then run make:
$ make

After a while, you should have the gdal library built. Check that libgdal-1.dll in .bin/ is present.
Then install it in the install/extra/ subdirectory of the project. if project is located in YourUsers/Documents/Displace, then use

$ make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install

Now you need to FIX the GDAL installation.
Linux and other Unix systems use subdirectories of include path to keep libraries header files separated. MinGW does not.
So we fix the header files installation:

cd /c/Users/YourUsers/Documents/Displace/install/extra
cd usr/local/include
mkdir gdal
mv * gdal


4) Compile GeographicLib

$ ./configure  --host=x86_64-w64-mingw32

version 1.39 may require some fix, in particular in include/GeographicLib/config.h where you should add the following lines:

#define GEOGRAPHICLIB_VERSION_MAJOR 1
#define GEOGRAPHICLIB_VERSION_MINOR 32
#define GEOGRAPHICLIB_VERSION_PATCH 0

and the following to include/GeographicLib/Constants.hpp:

#define GEOGRAPHICLIB_VERSION_STRING GEOGRAPHICLIB_VERSION

$ make
$ make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install


GDAL Notes
==========

Current version of QMapControl supports shapefiles loading ONLY for WGS84 Coordinates system. This is because QMapControl itself uses WGS84 coordinates.
So you need to convert your shapefiles if not using this CRS.
You can use ogr2ogr or the provided script in the scripts/ directory.

$ ogr2ogr -t_srs WGS84 DEST.shp SRC.shp

Note that DEST file is specified before the source!
