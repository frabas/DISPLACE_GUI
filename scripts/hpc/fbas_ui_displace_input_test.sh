#!/bin/bash

if [ $# -lt 2 ] ; then
    echo "Usage: $basename <sim_start_num> <sim_end_number>"
    echo "Example: $basename 4 8"
    exit
fi

a=$1
b=$2
for ((i=$a; i<=$b; i++)) do
    cat base_script_ui.txt > simu$i.sh
    chmod +rwx simu$i.sh
    # sed -i -e 's/fba_ibm_vessels/fba_ibm_vessels_simu'$i'/' simu$i.sh
    cat simu$i.sh | sed -e 's/fba_ibm_vessels/displace_test_simu'$i'/' > simu$i.sh.new
    mv simu$i.sh.new simu$i.sh
    echo "../DISPLACE_GUI/displace -f \"fake\" -f2 \"baseline\" -s \"simu$i\" -i 8762 -p 1 -o 1 -e 0 -v 0 --without-gnuplot -V 0 --num_threads 8" >> simu$i.sh
    qsub simu$i.sh
done


# from the solaris console (from the DTU G-BAR after running the ThinLinc client from WINBAR from remote deskop access), 
# type 'qrsh' or 'linuxsh' to connect the interactive node
# It is possible to compile the simulation module without QT, for example on an HPC.
# Just enter the simulation directory and run make:
# $ cd simulation
# $ make
# Installation need to be done manually, by copying the binary into the destination directory
# $ sudo install -m 0755 displace /usr/local/bin/
# ...make the file executable with chmod
# then execute the present bash script by ./fbas_ui_displace_input_test.sh that will connect an HPC execution node via 'qsub'
