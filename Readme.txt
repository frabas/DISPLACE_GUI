How to build
------------


Windows
-------

QT5 with MinGW is at the moment 32bit only.
Displace simulator requires to be compiled with a 64bit compiler, to allow access to more than 1.2G ram.
In the simulator projecy file (simulator.pro) the mingw64 compiler is named x86_64-w64-mingw32-g++ , 
instead of g++, to force you to correctly configure the project.
So before trying to compile under windows, you need to include the 64bit compiler path in the project.

Under Projects > select Displace project > Build Environment >

add the compiler's path to the END of the PATH variable, separated by ";"
For example: from
	PATH = C:\Qt\....
to
	PATH = C:\Qt\...;C:\mingw\x64-4.8.1-posix-seh-rev5\mingw64\bin

If you forget to append this, the compilation will fail with a "File not found" error.

