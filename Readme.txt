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

DISPLACE: a dynamic, individual-based model for spatial fishing planning and effort displacement 
integrating underlying fish population models, check out the paper 2014 in 
Canadian Journal of Fisheries and Aquatic Sciences

The DISPLACE project develops and provides a platform primarily for research purposes 
to transform the fishermens detailed knowledge into models, evaluation tools, and methods 
that can provide the fisheries with research and advice. 
The model intends to serve as a basis for decision support tools for (fishery) managers.

Among other goals, economic benefit of stock replenishment and sustainable harvesting 
can be demonstrated. This contributes to evaluate the combined ecological and economic impacts 
of fishery management before its implementation (i.e. impact assessment).

An individual-based model (IBM) on a per-vessel basis covering several fisheries and stocks 
is a benchmark tool capable of integrating fishermens decision-making processes 
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
As a stakeholder once said if you are not at the table, you are on the menu! 
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
under spatial restrictions.  ICES Journal of Marine Science, doi: 10.1093/icesjms/fsu215.


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

* Compilation of simulator only / without QT

It is possible to compile the simulation module without QT, for example on an HPC.

Just enter the simulation directory and run make:

$ cd simulation
$ make

Installation need to be done manually, by copying the binary into the destination directory

$ sudo install -m 0755 displace /usr/local/bin/

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

We Assume you already have QtCreator installed.

Notes: You can always use -jXXX on mingw32-make to build concurrently, if you have a molticore/multiprocessor system.


=== Install Qt 5.3.1 64 bit 

=== Install MSYS


********* NOTE ALL prefix under msys must be /mingw!!!!


MSYS is the command line shell for mingw. Download it from here:

http://sourceforge.net/projects/mingw-w64/files/External%20binary%20packages%20%28Win64%20hosted%29/MSYS%20%2832-bit%29/

Unpack msys in c:/Qt
Then you must configure msys with the previous mingw64 installation, so you need to run msys.bat, and run the following command

$ sh /postinstall/pi.sh

Answer "Y" to the following two questions, then enter the location of the mingw toolchain.
gcc is located on the mingw64/bin/ subdirectory of the mingw installation directory. If you selected c:\mingw64 and x86_64 / posix / seh toolchain, it will likely be something like

c:/Qt/qt-5.3.1-x64-mingw482r4-seh-opengl/mingw64

Anyway, check your installation for the right path. After restarting MSYS, you should be able to run gcc --version without errors.


=== Installing find

msys doesn't provide the find utility, and windows provides a very limited utility called FIND.EXE that is incompatible.
You need to manually install find: take it from here:

http://sourceforge.net/projects/mingw/files/MSYS/Base/findutils/

and copy the 3 files in msys/bin


=== Compile and install zlib 1.2.8

GDAL requires zlib to be a shared library, but unfortunately the previous package only provide it in form of a static library.
This way, only static gdal library will be compiled, and you'll be unable to link displace and the gdal utilities will bloat.

So download the zlib sources from http://www.zlib.net/, unpack it somewhere (I suggest some path under the mingw path) and compile

$ mingw32-make -f win32/Makefile.gcc -j12
...
$ cp zlib1.dll /c/Qt/qt-5.3.1-x64-mingw482r4-seh-opengl/mingw64/bin/
$ cp zconf.h zlib.h /c/Qt/qt-5.3.1-x64-mingw482r4-seh-opengl/mingw64include
$ cp libz.a /c/Qt/qt-5.3.1-x64-mingw482r4-seh-opengl/mingw64lib
$ cp libz.dll.a /c/Qt/qt-5.3.1-x64-mingw482r4-seh-opengl/mingw64lib/libz.dll.a


=== Compile GEOS 3.4.2

First download geos library from http://trac.osgeo.org/geos/ (at the time of writing, version is 3.4.2)
Unpack the tarbz2 file, enter the source dir and run configure

$ ./configure --prefix=/usr/local

Change capi/Makefile from

libgeos_c_la_CPPFLAGS = -DGEOS_CAPI_VERSION='$(GEOS_CAPI_VERSION)' -DGEOS_JTS_PORT='$(GEOS_JTS_PORT)'

to

libgeos_c_la_CPPFLAGS = 


then run make and make install

$ mingw32-make 
$ mingw32-make install

( Eventually: $ make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install  )

The first install instruction installs the necessary files in the mingw distribution (in /usr/local/) to allow gdal and all other libraries to find it during the configure process.
The second install instruction, instead, installs the library in the "distribution" path for use with qtCreator.




=== Compile GDAL with GEOS support

Enter the gdal source (they can be downloaded from: http://trac.osgeo.org/gdal/wiki/DownloadSource )
Use configure & make, as usual: 

$ ./configure  --prefix=/usr/local --disable-static --enable-shared \
	--with-geos=yes

FIX the compilation script: 
compiling under MinGW 64 requires link gdal with iconv: so open the GDALmake.opt file and change the following line:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL)

to:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL) -liconv


FIX the compilation script: 
compiling under MinGW 64 requires link gdal with iconv: so open the GDALmake.opt file and change the following line:

CONFIG_LIBS	=	$(GDAL_ROOT)/$(LIBGDAL) -liconv

Then, fix gcore/GnuMakefile: look for the gdaldrivermanager.$(OBJ_EXT) recipe, remove it and put this:
	
instdata.h: 
	echo "#define INST_DATA \"$(INST_DATA)\"" > $@

gdaldrivermanager.$(OBJ_EXT):	gdaldrivermanager.cpp ../GDALmake.opt instdata.h
	$(CXX) -c $(GDAL_INCLUDE) $(CXXFLAGS) \
		$< -o $@	

Then add the following

#include "instdata.h"

in gcore/gdaldrivermanager.cpp
Make analogous changes to ogr/ogrsfformats/generic/GNUMakefile, removing INST_DATA from CXXFLAGS: 

CXXFLAGS :=     $(CXXFLAGS) -DINST_DATA=\"$(INST_DATA)\" $(BASEFORMATS)

to 

CXXFLAGS :=     $(CXXFLAGS) $(BASEFORMATS)

and change 

#include "../../../gcore/instdata.h"

to ogr/ogrsfformats/generic/ogrsfdriverregistrar.cpp




Then run make:
$ mingw32-make

After a while, you should have the gdal library built. Check that libgdal-1.dll in .bin/ is present.
Then install

$ mingw32-make install

Then install it in the install/extra/ subdirectory of the project. if project is located in YourUsers/Documents/Displace, then use

$ mingw32-make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install

Now you need to FIX the GDAL installation.
Linux and other Unix systems use subdirectories of include path to keep libraries header files separated. MinGW does not.
So we fix the header files installation:

cd /c/Users/YourUsers/Documents/Displace/install/extra
cd usr/local/include
mkdir gdal
mv * gdal




=== Compile GeographicLib

$ ./configure  --prefix=/usr/local

version 1.39 may require some fix, in particular in include/GeographicLib/config.h where you should add the following lines:

#define GEOGRAPHICLIB_VERSION_MAJOR 1
#define GEOGRAPHICLIB_VERSION_MINOR 39
#define GEOGRAPHICLIB_VERSION_PATCH 0
#define GEOGRAPHICLIB_VERSION_STRING "1.39.0"
#define GEOGRAPHICLIB_DATA "/usr/local/share/GeographicLib"

$ mingw32-make
$ mingw32-make DESTDIR=/c/Users/YourUsers/Documents/Displace/install/extra install


=== Building boost

Download boost and download the following patch

http://www.kineticsystem.org/sites/default/files/mingw/patch/boost-1.56-bootstrap.patch

The apply the patch 

$ patch -p1 -i boost-1.56-bootstrap.patch

This patch applies flawlessly even on boost 1.57

Then bootstrap boost:

$ sh bootstrap.sh --with-toolset=mingw

and compile

$ b2 -j4 --prefix=$BOOST_PREFIX --disable-filesystem2 --with-graph --with-thread --with-system toolset=gcc /
    address-model=64 variant=release link=shared threading=multi install
	

	
=== Compiling GMP

$ ./configure --prefix=/mingw
$ make
$ make install

=== Compiling MPFR:

Patch with the latest patches (allpatch.txt).

$ ./configure --prefix=/mingw CFLAGS=-I/mingw/include CFLAGS=-I/mingw/include --disable-static --enable-shared
	
make 
make install
	
=== Compiling CGAL

Warning: CGAL requires two additional boost libraries: thread and system. If you are upgrading from a previous version, you'll need to install the missing components - see the boost section for details.

First, create the proper makefiles, because the sources are by default set up to create NMake makefiles.

$ cmake -g "mingw Makefiles"

Then configure using cmake
	
$ cmake . -DCMAKE_INSTALL_PREFIX=/mingw

Remove the CGAL_Qt3 and CGAL_Qt4 directories if the installer complains about missing QT3/QT4.

then make and make install

$ make
$ make install

DESTDIR can be used to change the destination directory.


GDAL Notes
==========

Current version of QMapControl supports shapefiles loading ONLY for WGS84 Coordinates system. This is because QMapControl itself uses WGS84 coordinates.
So you need to convert your shapefiles if not using this CRS.
You can use ogr2ogr or the provided script in the scripts/ directory.

$ ogr2ogr -t_srs WGS84 DEST.shp SRC.shp

Note that DEST file is specified before the source!
