How to build
------------


Windows
-------

QT5 with MinGW is at the moment 32bit only.
Displace simulator requires to be compiled with a 64bit compiler, to allow access to more than 1.2G ram.
So building under windows is somehow tricky.
First, you'll need to install both Qt5 with MinGW32, and a plain MinGW-64.

Gui can be compiled as usual and doesn't require any special procedure.

For Simulator, follow these tips:

- The mingw64 compiler is named x86_64-w64-mingw32-g++ , instead of g++, to force you to correctly configure
    the project.

- Add the mingw64 compiler's path to the path of the building environment:
    Projects > select Displace project > Build Environment >
    add the compiler's path to the head of the PATH variable, separated by ";"

    for example: C:\mingw\x64-4.8.1-posix-seh-rev5\mingw64\bin;etc...

- Select the Run Environment ("Run" tab under Projects page) to be the same as
    the build environment

- Select the output path of the simulator to be a subdirectory of the main project path.
    For example, setup.iss requires the project to be built under
        (projectdir)/build/release
    so define the output path of the simulator project to be
        (projectdir)/build/release/simulator

- Due to conflicting libraries, Simulator will be linked statically to minGW64.

