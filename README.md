Find your way with DISPLACE
======

## Summary
- [What is for?](#what-is-for)
- [How to contribute](#how-to-contribute)
- [How to install](#how-to-install)
- [Quick start for running a DISPLACE simulation](#quick-start-for-running-a-displace-simulation)
- [Simulation output formats](#simulation-output-formats)
- [How to build from scratch](#how-to-build-from-scratch)
- [DISPLACE doxygen documentation](#displace-doxygen-documentation)
- [GDAL notes](#gdal-notes)
- [Unit testing](#unit-testing)
- [Build a new case study](#build-a-new-case-study)
- [How to cite](#how-to-cite)



## What is for?


DISPLACE is a dynamic, individual-based model for spatial fishing planning and effort displacement 
integrating underlying fish population models. The DISPLACE project develops and provides a 
platform primarily for research purposes to transform the fishermen detailed knowledge into models,
evaluation tools, and methods that can provide the fisheries with research and advice. 
By conducting a ecological and socio-economic impact assessment the model intends to serve as an aid to decision-making for (fishery) managers.
An impact assessment will help answering on what are the impacts of the different policy options and who will be affected.
By quantifying the effects the assessment will measure how the different options compare, for example 
how different the options perform in achieving the objective(s) of the policy.

## How to contribute

[Please read our CONTRIBUTING statement here](CONTRIBUTING.md)


## How to install DISPLACE


Look at the [Release section](https://github.com/frabas/DISPLACE_GUI/releases) on this GitHub repository 
to download an installer for Windows. Alternatively, look at the 
[google drive for DISPLACE](https://drive.google.com/drive/folders/0ByuO_4j-1PxtfnZBblpQNmh2a2Z4SmpkRC16T1kwR0t1RWUyOVUxdHlEZzZwZWVpaVJac00)
for Unix or MacOSX packages, also hosting additional files i.e. possible dependencies and the DISPLACE software development kit.


### Install on Windows

Launch the installer application, and follow the guide. There are no prerequisites on Windows, and the application
should work out of the box.

### Install on MacOS

Open the DMG file, then drop the program in the Application folder.
There are no prerequisites on MacOS, and the application should work out of the box.

### Install on Ubuntu Linux

Ubuntu 18.04LTS has a few prerequisites that must be installed before installing the displace package itself.

Run the following command to install the prerequisites:

```bash

$ sudo apt install libgdal20 libgdal-dev libcgal13 libcgal-dev libboost1.65-all-dev libgeographic17 libqt5gui5 libqt5widgets5 libqt5xml5

```

Then install the `msqlitecpp` packages provided in the download section:

```bash
$ sudo dpkg -i msqlitecpp0_0.9.4_amd64.deb msqlitecpp-dev_0.9.4_amd64.deb 
```


Finally, install the displace package:

```bash
$ sudo dpkg -i displace_0.9.22_amd64.deb
```

If you have any difficulty, try fixing the package dependencies by running: 

```bash
$ sudo apt --fix-broken install
```

Any missing package should be automatically installed.

## How to compile from the code source


[compiling with CMake (preferred)](docs/Building-cmake.md)

[compiling on HPC (simulator only)](docs/Building-on-hpc.md)

[making the displace sdk (optional)](docs/building.md)

[compiling on Windows (deprecated)](docs/Building.win)

[compiling on Unix deprecated)](docs/Building.unix)

[compiling on MacOSX deprecated)](docs/Building.MacOSX)


## DISPLACE doxygen documentation

Can be found [here](https://frabas.github.io/DISPLACE_GUI/doxygen/doc/html/index.html)

Procedure for updating the doxygen documentation without keeping track of git history [here](https://github.com/robotology/how-to-document-modules/blob/master/README.md)

## Quick start for running a basic DISPLACE simulation

[DISPLACE Example datasets](https://displace-project.org/blog/download/) are available for download. 
You need to unzip the downloaded file to a folder that name the dataset with the pattern DISPLACE_input_xx,
for example DISPLACE_input_minitest which is the minimal dataset typically used for demonstration purpose.


Run DISPLACE with e.g. displacegui


By default the Model Objects is set to 4. If you want to run a scenario, first make sure your Model Objects is set to [0].


![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_select_model_0.png)


If yes then in the main menu do a "File">"Load a Scenario Model, 


 ![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_load_scenario_file.png)


and choose a scenario file (a .dat file) you´ll find in the\simusspe subfolder of your DISPLACE dataset.
Select the file, click Ok and wait to see the DISPLACE graph plotted on the map.


 ![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_scenario_file_is.png)


You can now click Start in the DISPLACE command panel for a DISPLACE simulation to start:


 ![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_click_start.png)

After some object creation and initialization the time step window will shortly update and the simulation run to the end time step. 
By default, 8762 hourly time steps will be simulated which is approx. the number of hours in one year.
in Setup menu the total number of time step can be changed to up to 52586 for a maximal 6-years horizon. 
Because of the computation time, running more than one year simulation and many replicates are better done on a HPC cluster.
Automated shell scripts to run many DISPLACE simulations in parallel on a HPC cluster can be provided on request. 

 ## Simulation output formats


Look at the [description](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/output_fileformats.md) of the list of files produced by a DISPLACE simulation. 
A [displaceplot](https://github.com/frabas/displaceplot/releases) R package has been developed to handle these output text files and produced some plots out of them. 
Simulation outcomes are also exported as a SQLite database which can be re-loaded within DISPLACE in a Replay mode. The internal structure and simulated data
can be further retrieved from the database when using an external SQLite DB browser. 

To load a result database into DISPLACE: 



 ![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_load_db_menu.png)

Select a DISPLACE db file:

 ![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_load_db_file.png)

Once loaded, the simulation can be replayed with the Replay command:

 ![alt text](https://github.com/frabas/DISPLACE_GUI/blob/master/docs/quickstart_replay_command.png)




## GDAL Notes


Current version of QMapControl supports shapefiles loading ONLY for WGS84 Coordinates system. This is because QMapControl itself uses WGS84 coordinates.
So you need to convert your shapefiles if not using this CRS.
You can use ogr2ogr or the provided script in the scripts/ directory.

$ ogr2ogr -t_srs WGS84 DEST.shp SRC.shp

Note that DEST file is specified before the source!


## Unit testing


Unit testing is performed using the Boost::Test framework. It can be compiled and linked in two ways:

- Dynamic linked, using the system installed boost::test library. This option is enabled by default on Unix
- Compiled in, using the boost/test/included/unit_test.hpp (included in main.cpp). This is suitable for Windows, where boost::test is not available with our version of mingw64. A standard installation of boost::test is required.

The two methods can be selected by defining boost_test_included in the CONFIG line of the Qt Project. It is enabled by default on Windows (see localconfig.pri).

If boost::test is not available in any form, it can be disabled by removing the units-test option from the CONFIG variable in the pro file.

## Build a new case study

A set of [R routines](https://github.com/frabas/DISPLACE_R_inputs) are available to create a new DISPLACE case study from scratch. These routines are quite data-hungry 
and would require to be adapted to each case. Ideally a R package should be build soon to wrap them in a consistent tool. 
Another way is to scrutinize the DISPLACE_input_mintest github repository to see all the required input files required 
to run a minimal case study. Because it can be currently tricky to understand the logic behind all the input text files 
we will think one day to build a unique input database to DISPLACE instead of using so many individual text files.

## How to cite

* Bastardie F, Nielsen JR, Miethe T. 2014. DISPLACE: a dynamic, individual-based model for 
spatial fishing planning and effort displacement - integrating underlying 
fish population models. Canadian Journal of Fisheries and Aquatic Sciences. 71(3):366-386. [link here](https://www.nrcresearchpress.com/doi/full/10.1139/cjfas-2013-0126#.XJs-ubh7nmE) 

* Bastardie, F., Nielsen, J. R., Eigaard, O. R., Fock, H. O., Jonsson, P., and Bartolino, V. 
Competition for marine space: modelling the Baltic Sea fisheries and effort displacement 
under spatial restrictions. ICES Journal of Marine Science, doi: 10.1093/icesjms/fsu215. [link to a free copy](https://academic.oup.com/icesjms/article/72/3/824/701817) 

* Bastardie, F., Nielsen, J. R., Eero, M., Fuga, F. Rindorf., A. 2017. Effects of changes
in stock productivity and mixing on sustainable fishing and economic viability,
ICES Journal of Marine Science, Volume 74, Issue 2, Pages 535–551
[link to a free copy](https://academic.oup.com/icesjms/article/74/2/535/2669542)

* Bastardie, F., Angelini, S., Bolognini, L., Fuga, F., Manfredi, C., Martinelli, M.,
Nielsen, J. R., Santojanni, A., Scarcella, G., and Grati, F.. 2017. 
Spatial planning for fisheries in the Northern Adriatic: working toward viable and sustainable fishing.
Ecosphere 8( 2):e01696. [link to a free copy](https://esajournals.onlinelibrary.wiley.com/doi/full/10.1002/ecs2.1696) 

