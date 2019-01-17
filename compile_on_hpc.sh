#!/bin/bash

clear
echo "Compiling DISPLACE with cmake..."

cd Build


/appl/cmake/3.12.2/bin/cmake .. -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT="/appl/boost/1.64.0-gcc-6.3.0/" -DCMAKE_CXX_COMPILER="/appl/gcc/7.3.0/bin/g++" -DCMAKE_C_COMPILER="/appl/gcc/7.3.0/bin/gcc" -DGeographicLib_LIBRARIES="~//local/geographiclib-1.49/bin/" -DGeographicLib_INCLUDE_DIRS="~//local/geographiclib-1.49/include/" -DGeographicLib_LIBRARY_DIRS="~//local/geographiclib-1.49/lib/" -DCMAKE_MODULE_PATH="~//local/geographiclib-1.49/share/cmake/GeographicLib/" -DSPARSEPP_ROOT="~//ibm_vessels/DISPLACE_GUI/sparsepp/" -DCMAKE_PREFIX_PATH="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/lib64/cmake/msqlitecpp/" -DWITHOUT_GUI=On

      





