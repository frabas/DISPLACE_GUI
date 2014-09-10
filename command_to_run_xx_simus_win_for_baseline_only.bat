

REM 5 years horizon is up to 43824 steps
REM 1 year horizon is up to 8761 steps


REM SCE BASELINE ----------------------------------


 for /l %%a in (2,1,10) do (start /d C:\Users\fba\displace.exe -f "balticonly" -f2 "baseline" -s "simu%%a" -i 43824 -p 1 -o 1 -e 0 --without-gnuplot 
 PING 1.1.1.1 -n 1 -w 100000 >NUL) 
 REM delay the next command by 300 seconds because the srand c++ seed init is changing every second only
 REM i.e. it is important to do not start several simulations within the same second in order to get the stochasiticity done

