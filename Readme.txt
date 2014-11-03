README
======

Summary
- How to build
- GDAL notes


How to build
============


Windows
-------

Since version 0.5.0 Displace requires GDAL library.
It must be properly built under MinGW. See the proper section below, because there's no a

QT5 with MinGW is at the moment 32bit only.
Displace simulator requires to be compiled with a 64bit compiler, to allow access to more than 1.2G ram.
In the simulator projecy file (simulator.pro) the mingw64 compiler is named x86_64-w64-mingw32-g++ , 
instead of g++, to force you to correctly configure the project.
So before trying to compile under windows, you need to include the 64bit compiler path in the project.

Under Projects > select Displace project > Build Environment >

add the compiler's path to the END of the PATH variable, separated by ";"
For example: from
	PATH = C:\Qt\....
to
	PATH = C:\Qt\...;C:\mingw\x64-4.8.1-posix-seh-rev5\mingw64\bin

If you forget to append this, the compilation will fail with a "File not found" error.

GDAL Installation under MinGW
-----------------------------

Install MSYS. Compilation must be performed from the MSYS shell, so you must have a working MSYS system with proper toolchain and autotools.

From the MSYS shell, enter the gdal-1.x.x directory.
Then build the library as normally done with Gnu projects:

$ ./configure  --host=x86_64-w64-mingw32 --disable-static --enable-shared

FIX the compilation script: 
compiling under MinGW 64 requires link gdal with iconv: so open the GDALmake.opt file and change the following line:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL)

to:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL) -liconv

Then run make:
$ make

After a while, you should have the gdal library built.
Then install it in the install/extra/ subdirectory of the project. if project is located in YourUsers/Documents/Displace, then use

$ make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install

Now you need to FIX the GDAL installation.
Linux and other Unix systems use subdirectories of include path to keep libraries header files separated. MinGW does not.
So we fix the header files installation:

cd /c/Users/YourUsers/Documents/Displace/install/extra
cd usr/local/include
mkdir gdal
mv * gdal

A warning will appear, telling that gdal can't be moved into gdal/gdal. It's ok, ignore it.


GDAL Notes
==========

Current version of QMapControl supports shapefiles loading ONLY for WGS84 Coordinates system. This is because QMapControl itself uses WGS84 coordinates.
So you need to convert your shapefiles if not using this CRS.
You can use ogr2ogr or the provided script in the scripts/ directory.

$ ogr2ogr -t_srs WGS84 DEST.shp SRC.shp

Note that DEST file is specified before the source!
