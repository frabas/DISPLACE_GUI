#!/bin/sh

PATH=`pwd`/build/release-qt5/
MOD=testexample
MODEL=baseline
SIMU=simu2
STEPS=8762

LD_LIBRARY_PATH=${PATH}

if [ "$1" != "" ] ; then
  MOD=$1
fi
if [ "$2" != "" ] ; then
  MODEL=$2
fi

SIM=/home/happycactus/Documents/Progetti/displace/Applications/DISPLACE_input_${MOD}

cd $SIM
${PATH}/displace -f ${MOD} -f2 ${MODEL} -s ${SIMU} -i ${STEPS} -p 1 -o 1 -e 1 -v 0 --without-gnuplot -V 1 --num_threads 1

