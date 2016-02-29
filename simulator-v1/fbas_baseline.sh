#!/bin/bash

if [ $# -lt 2 ] ; then
    echo "Usage: $basename <sim_start_num> <sim_end_number>"
    echo "Example: $basename 4 8"
    exit
fi

a=$1
b=$2
for ((i=$a; i<=$b; i++)) do
    cat base_script > simu$i.sh
    chmod +rwx simu$i.sh
    # sed -i -e 's/fba_ibm_vessels/fba_ibm_vessels_simu'$i'/' simu$i.sh
    cat simu$i.sh | sed -e 's/fba_ibm_vessels/fba_base_simu'$i'/' > simu$i.sh.new
    mv simu$i.sh.new simu$i.sh
    echo "../ibm_vessels_cpp/ibm_vessels -f \"balticonly\" -f2 \"baseline\" -s \"simu$i\" -i 43824 -p 1 -o 1 -e 0 -v 0 --without-gnuplot" >> simu$i.sh
    qsub simu$i.sh
done


# from the solaris console (from the DTU G-BAR after running the ThinLinc client from WINBAR from remote deskop access), 
# type 'qrsh' to connect the interactive node
# then compile the c++ code with ./compile.sh
# then execute the present bash script by ./fbas_baseline.sh that will connect an HPC execution node via 'qsub'
