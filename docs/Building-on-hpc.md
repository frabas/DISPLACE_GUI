# Building displace on HPC (High Performance Computing)

## Dependencies

First we need the GeographicLib, 
download at https://geographiclib.sourceforge.io/html/install.html#cmake

open a HPC console
```bash
linuxsh
```

```bash
tar xfpz GeographicLib-1.49.tar.gz
cd GeographicLib-1.49/
./configure --prefix=$HOME/local/geographiclib-1.49
make -j 8
make install
```
then it should be now installed in ~//local/ on the HPC

likely it is needed to manually cp GeographicLib.so* in the bin directory


## Compiling DISPLACE (simulator only)

```bash
cd DISPLACE_GUI
mkdir Build 
cd Build
```
compile from the DISPLACE_GUI folder with such a bash script:

```bash
#!/bin/bash

clear
echo "Compiling DISPLACE with cmake..."

cd Build

/appl/cmake/3.12.2/bin/cmake .. -DCMAKE_BUILD_TYPE=Release 
-DBOOST_ROOT="/appl/boost/1.64.0-gcc-6.3.0/" 
-DCMAKE_CXX_COMPILER="/appl/gcc/7.3.0/bin/g++"
-DCMAKE_C_COMPILER="/appl/gcc/7.3.0/bin/gcc" 
-DGeographicLib_LIBRARIES="~//local/geographiclib-1.49/lib/libGeographic.so" 
-DGeographicLib_INCLUDE_DIRS="~//local/geographiclib-1.49/include/" 
-DGeographicLib_LIBRARY_DIRS="~//local/geographiclib-1.49/lib/"  
-DCMAKE_MODULE_PATH="~//local/geographiclib-1.49/share/cmake/GeographicLib/" 
-DSPARSEPP_ROOT="~//ibm_vessels/DISPLACE_GUI/sparsepp/" 
-DCMAKE_PREFIX_PATH="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/lib64/cmake/msqlitecpp/" 
-DWITHOUT_GUI=On
```

then build displace with:

```bash
make displace
```
