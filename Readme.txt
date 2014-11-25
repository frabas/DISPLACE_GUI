README
======

Summary
- Rationale
- How to cite
- How to install
- How to build
- GDAL notes



Rationale
============

Dynamic Individual vessel-based Spatial fishing PLAnning and effort displaCEment

DISPLACE: a dynamic, individual-based model for spatial fishing planning and effort displacement –
integrating underlying fish population models, check out the paper 2014 in 
Canadian Journal of Fisheries and Aquatic Sciences

The DISPLACE project develops and provides a platform primarily for research purposes 
to transform the fishermen’s detailed knowledge into models, evaluation tools, and methods 
that can provide the fisheries with research and advice. 
The model intends to serve as a basis for decision support tools for (fishery) managers.

Among other goals, economic benefit of stock replenishment and sustainable harvesting 
can be demonstrated. This contributes to evaluate the combined ecological and economic impacts 
of fishery management before its implementation (i.e. impact assessment).

An individual-based model (IBM) on a per-vessel basis covering several fisheries and stocks 
is a benchmark tool capable of integrating fishermen’s decision-making processes 
when they face changes in fishery management, economic factors influencing the fishery, 
economic viability, and underlying stock conditions, including spatial and 
seasonal patterns in resource availability.

DISPLACE operates with high resolution in time and space (spatio-temporal explicit). 
It allows contributing to marine/maritime spatial planning for e.g. evaluating 
the effects of smaller and larger marine area restrictions on stocks and fisheries, 
i.e. impact assessment on stocks and fisheries of broader marine management. 
The economic evaluation includes evaluation of spatial restrictions for the fishery 
with particular emphasis to test if there are any economic benefits or 
consequences from different area restrictions caused by e.g. renewable energy, conservation, etc.
Also it evaluates whether these benefits compensate the additional costs of 
the effort displaced to surrounding or new areas, i.e. the bio-economic consequences 
of fishing effort displacement.

This ultimately incorporates other utilization of the sea such as energy production, 
transport, recreational use, etc., e.g. offshore windmill farms, 
large marine constructions, NATURA 2000 areas, transport routes of 
commercial shipping, pipelines, cables, etc.

Fishermen communities have an obvious stake in using the sea to ensure food supply with seafood 
and should not then be left out the maritime spatial planning process. 
As a stakeholder once said “if you are not at the table, you are on the menu!” 
While fishermen are therefore likely to gain at designing their own priorities areas, 
the DISPLACE model should be a support for evaluating and balance these important areas 
against other potential uses and the economic consequences 
of spatial plans on fishermen´s living conditions.


How to cite
============

* Bastardie F, Nielsen JR, Miethe T. 2014. DISPLACE: a dynamic, individual-based model for 
spatial fishing planning and effort displacement - integrating underlying 
fish population models. Canadian Journal of Fisheries and Aquatic Sciences. 71(3):366-386. 
Available from: 10.1139/cjfas-2013-0126

* Bastardie, F., Nielsen, J. R., Eigaard, O. R., Fock, H. O., Jonsson, P., and Bartolino, V. 
Competition for marine space: modelling the Baltic Sea fisheries and effort displacement 
under spatial restrictions. – ICES Journal of Marine Science, doi: 10.1093/icesjms/fsu215.


How to install
============


Look at the Release section on the GitHub repository 
to download an installer for Windows. 

An example dataset is also available (DISPLACE_input)

or alternatively, look at the google drive for DISPLACE
also hosting additional files



How to build
============

Linux
-----

* Prerequisites:

The following libraries, with development files, are required to compile under linux:

Gdal library 1.10
Boost Graph library 1.55 
GeographicLib 

To install prerequisites under Ubuntu 14.10:

$ sudo apt-get install libgdal1-dev
$ sudo apt-get install libboost-graph1.55-dev
$ sudo apt-get install libgeographiclib-dev

* Compilation under linux

Using QtCreator is the simplest way: load the project file, configure the build directory and click on Build All.
From QtCreator you can also run the application without any issue. Just be sure to select qtgui as run object.

Using the command line is simple as well. Create a new directory to receive the objects files, and then run qmake and make from there:

$ cd displace
$ mkdir build-release
$ cd build-release
$ qmake ../
$ make

Installing
To install, use make this way:

$ make install

will install in the unix "local" path /usr/local
to change prefix, use PREFIX variable:

$ make PREFIX=/usr/

will install in the package-reserved path /usr.
Package Maintainer can use INSTALL_ROOT variable to deploy on a separate directory:

$ make PREFIX=/usr/ INSTALL_ROOT=$PWD/install

Make uninstall is used the same way. 

NOTE: 
Be sure to use qmake for Qt5. In case of Ubuntu 14.10, be sure to install qt5-default to automatically select qmake for qt5, otherwise qmake will point to the qt4 version.

Windows
-------

* Prerequisites:

Since version 0.5.0, Displace requires GDAL library, with GEOS support
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

$ export PATH=/c/Qt/qt-5.3.2-x64-mingw491r1-seh-opengl/mingw64/bin:$PATH

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

3) Compile GEOS

First download geos library from http://trac.osgeo.org/geos/ (at the time of writing, version is 3.4.2)
Unpack the tarbz2 file, enter the source dir and run configure

$ ./configure  --host=x86_64-w64-mingw32 --prefix=/usr/local

then run make and make install

$ make 
$ make install
$ make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install

The first install instruction installs the necessary files in the mingw distribution (in /usr/local/) to allow gdal and all other libraries to find it during the configure process.
The second install instruction, instead, installs the library in the "distribution" path for use with qtCreator.

4) Compile GDAL with GEOS support

Enter the gdal source (they can be downloaded from: http://trac.osgeo.org/gdal/wiki/DownloadSource )
Use configure & make, as usual: 

$ ./configure  --host=x86_64-w64-mingw32 --prefix=/usr/local --disable-static --enable-shared \
	--with-geos=yes

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


5) Compile GeographicLib

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
