#!/bin/sh

help() {
  cat << __EOF
Usage: memtest.sh [-p path] [-m|-c]  [-h] [MOD] [MODEL]

  -p path     Run an alternative binary (build/release-qt)
  -m          Run massif
  -c          Run memcheck
  --steps n   Run n steps
  -h          Show this help

  MOD         The mod (testexample)
  MODEL       The config (baseline)
__EOF
}


SRC=`pwd`
MYPATH=`pwd`/build/release-qt5/
MOD=testexample
MODEL=baseline
SIMU=simu2
STEPS=8762

while [ "$1" != "" ] ; do
  case "$1" in
    -m)
      RUN="valgrind --tool=massif --massif-out-file=${SRC}/massif.out"
      shift
      ;;
    -c)
      RUN="valgrind --tool=memcheck --log-file=${SRC}/memcheck.out"
      shift
      ;;    
    -p)
      shift
      MYPATH=`realpath $1`
      shift
      ;;
    --steps)
      shift
      STEPS=$1
      shift
      ;;
    -h)
      help
      exit 1
      ;;
    *)
      break
      ;;
  esac
done

if [ "$1" != "" ] ; then
  MOD=$1
fi
if [ "$2" != "" ] ; then
  MODEL=$2
fi

export LD_LIBRARY_PATH=${MYPATH}

SIM=/home/happycactus/Documents/Progetti/displace/Applications/DISPLACE_input_${MOD}

cd $SIM
${RUN} ${MYPATH}/displace -f ${MOD} -f2 ${MODEL} -s ${SIMU} -i ${STEPS} -p 1 -o 1 -e 1 -v 0 --without-gnuplot -V 1 --num_threads 1

