#!/bin/bash

clear
echo "Compiling mSqliteCpp..."

# first clone on github.com/studiofuga/mSqliteCpp...

cd ../mSqliteCpp
mkdir Build
cd Build

/appl/cmake/3.12.2/bin/cmake .. -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT="/appl/boost/1.64.0-gcc-6.3.0/" -DCMAKE_CXX_COMPILER="/appl/gcc/7.3.0/bin/g++" -DCMAKE_C_COMPILER="/appl/gcc/7.3.0/bin/gcc" -DENABLE_TEST=Off -DENABLE_PROFILER=Off

make DESTDIR="/zhome/fe/8/43283/local/mSqliteCpp/" install

