#!/bin/bash

clear
echo "Compiling DISPLACE with cmake..."

cd Build

#Dmsqlitecpp_DIR="/zhome/fe/8/43283/ibm_vessels/mSqliteCpp/cmake/"

/appl/cmake/3.12.2/bin/cmake .. -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT="/appl/boost/1.64.0-gcc-6.3.0/" -DCMAKE_CXX_COMPILER="/appl/gcc/7.3.0/bin/g++" -DCMAKE_C_COMPILER="/appl/gcc/7.3.0/bin/gcc" -DGEOGRAPHIC_INCLUDE="/zhome/fe/8/43283/local/geographiclib-1.49/include/" -DGEOGRAPHIC_LIBDIR="~//local/geographiclib-1.49/lib/" -DCMAKE_PREFIX_PATH="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/lib64/cmake/msqlitecpp/" -DWITHOUT_GUI=On

#-DMSQLITECPP_INCLUDE="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/include/" -DMSQLITECPP_LIBDIR="/zhome/fe/8/43283/local/mSqliteCpp/usr/local/lib64/cmake/msqlitecpp"
      





