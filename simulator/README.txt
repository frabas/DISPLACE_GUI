DISPLACE_GUI
============

The DISPLACE model with a Graphical User Interface

Based on the c++/Qt framework

command line, for example:

displace -f "balticonly" -f2 "baseline" -s "simu1" -i 36482 -p 1 -o 1 -e 1 -v 0 --without-gnuplot

or:

displace     -f "balticonly" -f2 "baseline" -s "simu2" -i 8761 -p 1 -o 1 -e 0 -v 0 --without-gnuplot  // disable the VMS file exporting, the most used.
displace     -f "balticonly" -f2 "baseline"  -s "simu2" -i 8761 -p 1 -o 1 -e 1 -v 0 --with-gnuplot    // enable the VMS file exporting
displace     -f "balticonly" -f2 "baseline"  -s "simu2" -i 8761 -p 1 -o 1 -e 1 -v 1 --with-gnuplot    // subset of vessels, see features.dat
displace     -f "balticonly" -f2 "baseline"  -s "simu2" -i 8761 -p 1 -o 0 -e 1 -v 0 --with-gnuplot    // CAUTION create the path shop, need a lot of computation time
displace     -f "balticonly" -f2 "baseline"  -s "simu2" -i 8761 -p 0 -o 0 -e 1 -v 0 --with-gnuplot    // here, dynamic path building: use with care because need much more computation time...


TO DO: 

* develop a GUI including a map editor and geospatial data
* the code should be modified to make possible a call without the GUI
* the executable name should be modified to be "displace"
* add an option to deactivate the GUI

USING WITH BASH/SHELL
=============


on linux,
for using on High Computing Performance (HPC) service

use for example:
'fbas_baseline.sh' for the baseline scenario
which call the
'base_script' file

on Windows,
use for example
'command_to_run_XX_simus_win_for_baseline_only.bat'


write one shell .sh file or .bat per scenario


