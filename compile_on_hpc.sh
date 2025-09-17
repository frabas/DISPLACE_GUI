#!/bin/bash

clear
echo "Compiling DISPLACE with cmake..."

cd Build


# up to summer 2024  it was using /appl/cmake/3.12.2/bin/
# now cmake is in /appl9 and is called with module (see module av cmake):
#module load cmake/3.29.2  
#cmake .. -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT="/appl/boost/1.64.0-gcc-6.3.0/" -DCMAKE_CXX_COMPILER="/appl/gcc/7.3.0/bin/g++" -DCMAKE_C_COMPILER="/appl/gcc/7.3.0/bin/gcc" -DGeographicLib_LIBRARIES="~//local/geographiclib-1.49/lib/libGeographic.so" -DGeographicLib_INCLUDE_DIRS="~//local/geographiclib-1.49/include/" -DGeographicLib_LIBRARY_DIRS="~//local/geographiclib-1.49/lib/"  -DCMAKE_MODULE_PATH="~//local/geographiclib-1.49/share/cmake/GeographicLib/" -DSPARSEPP_ROOT="~//ibm_vessels/DISPLACE_GUI/sparsepp/" -DCMAKE_PREFIX_PATH="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/lib64/cmake/msqlitecpp/" -DWITHOUT_GUI=On

#module load cmake/4.0.3
module load cmake/3.27.9  
cmake .. -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT="/appl/boost/1.83.0-gcc-10.5.0/" -DCMAKE_CXX_COMPILER="/appl/gcc/10.5.0-binutils-2.40/bin/g++" -DCMAKE_C_COMPILER="/appl/gcc/10.5.0-binutils-2.40/bin/gcc" -DGeographicLib_LIBRARIES="~//local/geographiclib-2.5.2/lib/libGeographicLib.so.26.1.3" -DGeographicLib_INCLUDE_DIRS="/zhome/fe/8/43283/local/geographiclib-2.5.2/include/" -DGeographicLib_LIBRARY_DIRS="/zhome/fe/8/43283/local/geographiclib-2.5.2/lib/"  -DCMAKE_MODULE_PATH="~//local/geographiclib-2.5.2/share/cmake/GeographicLib/" -DSPARSEPP_ROOT="/zhome/fe/8/43283/ibm_vessels/DISPLACE_GUI/sparsepp/" -DCMAKE_PREFIX_PATH="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/lib64/cmake/msqlitecpp/" -DWITHOUT_GUI=On
      





