REM 8762 tsteps for one year....
cd "C:\Users\fbas\Documents\GitHub\DISPLACE_GUI\build\release" 
for /l %%a in (2,1,4) do (start /d C:\Users\fbas\Documents\GitHub\DISPLACE_GUI\build\release displace -f "fake" -f2 "baseline" -s "simu%%a" -i 100 -p 1 -o 1 -e 1 -v 0 --without-gnuplot -V 1 --num_threads 1 > ..\\out.txt
PING 1.1.1.1 -n 1 -w 10000 >NUL) 
PAUSE