# Displace – Quick Start

This guide helps you set up a development environment and build Displace on Windows (primary), with notes for Linux and macOS. The project uses CMake and vcpkg in manifest mode.

## Prerequisites

- CMake ≥ 3.5 (recommended: latest stable)
- A C++17 compiler
  - Windows: MSVC (Visual Studio 2022)
  - Linux: GCC ≥ 9 or Clang ≥ 10
  - macOS: AppleClang (Xcode command line tools)
- Git
- Qt 6.8.x (see “External dependencies: Qt6”)

## Project dependencies

From CMake modules:
- Boost (components: date_time, filesystem, system, thread, program_options, log, unit_test_framework)
- GeographicLib
- GDAL (minimum 1.11; some modules require newer GDAL, e.g., 2.20)
- Qt6 6.8 (modules: Core, Widgets, Concurrent, Sql, Xml, PrintSupport, Network) — when GUI is enabled
- CGAL — when GUI is enabled
- Sparsepp
- msqlitecpp (via overlay port)

From vcpkg manifest (vcpkg.json):
- gdal
- boost-geometry
- plus any additional ports declared in the manifest

Notes:
- The project defines BOOST_ALL_NO_LIB to avoid auto-linking on MSVC.
- An overlay port is provided for msqlitecpp under vcpkg-overlays/.

## vcpkg (manifest mode) – installation

1. Clone vcpkg:
   - Windows (PowerShell/CMD):
     ```
     git clone https://github.com/microsoft/vcpkg %USERPROFILE%\vcpkg
     %USERPROFILE%\vcpkg\bootstrap-vcpkg.bat
     ```
   - Linux/macOS (bash):
     ```
     git clone https://github.com/microsoft/vcpkg ~/vcpkg
     ~/vcpkg/bootstrap-vcpkg.sh
     ```

2. Set VCPKG_ROOT env var (optional but convenient):
   - Windows:
     ```
     setx VCPKG_ROOT "%USERPROFILE%\vcpkg"
     ```
   - Linux/macOS:
     ```
     echo 'export VCPKG_ROOT="$HOME/vcpkg"' >> ~/.bashrc
     ```

3. Toolchain path (used by CMake):
   - `%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake` (Windows)
   - `$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake` (Linux/macOS)

#### Note:
The repository includes vcpkg-configuration.json and vcpkg.json (manifest mode). However, top-level CMakeLists.txt currently sets:
    ```
    - VCPKG_MANIFEST_INSTALL OFF
    ```
This disables auto-install during configure. See next section for how to install dependencies manually or override this setting.

## Generate and install dependencies (manifest mode)

#### Important Note:
If you already have a vcpkg export of the dependencies (see “Using exported vcpkg dependencies”), you do not need to generate/install them again. Just point CMake/vcpkg to the exported installed directory as described below. Follow the steps in this section only when an export is not available or is incomplete/outdated.


### Option A — Manual install (recommended with the current CMake default):

- Open a shell in the project root (where vcpkg.json is).
- Choose a triplet (Windows: x64-windows; Linux: x64-linux; macOS: x64-osx).
- If you rely on overlay ports (e.g., msqlitecpp), ensure overlays are picked up via vcpkg-configuration.json. If needed, add:
    ```sh
    --overlay-ports=./vcpkg-overlays
    ```
 

- Install:
  - Windows:
    ```
    %VCPKG_ROOT%\vcpkg.exe install --triplet x64-windows
    ```
  - Linux/macOS:
    ```
    $VCPKG_ROOT/vcpkg install --triplet x64-linux   # or x64-osx
    ```

### Option B — Enable auto-install during CMake configure:

- Pass `VCPKG_MANIFEST_INSTALL=ON` in your CMake configure step:
  ```
    -DVCPKG_MANIFEST_INSTALL=ON
  ```

## External dependencies: Qt6

Displace requires Qt 6.8.x with these modules:
- Core
- Widgets
- Concurrent
- Sql
- Xml
- PrintSupport
- Network

You have two options:

1) Install Qt from the Qt online installer (recommended on Windows):
- Install Qt 6.8.x for your compiler (e.g., MSVC 2022).
- Set CMAKE_PREFIX_PATH to the Qt 6.8 installation, e.g.:
  - Windows (PowerShell/CMD):
    ```
    -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
    ```
  - Linux:
    ```
    -DCMAKE_PREFIX_PATH="/opt/Qt/6.8.0/gcc_64"
    ```
  - macOS:
    ```
    -DCMAKE_PREFIX_PATH="~/Qt/6.8.0/macos"
    ```

2) Install Qt via vcpkg (advanced; ensure the triplet and Qt platform plugins are correct):
- Add/enable Qt6 packages in vcpkg.json and run vcpkg install.
- Make sure the Qt6 platform plugin (e.g., windows, xcb, cocoa) is available at runtime.

## Configure and build

Windows (MSVC, x64-windows):

```sh
cmake -S . -B build ^
-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake ^
-DVCPKG_MANIFEST_INSTALL=OFF ^
-DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64" ^
-DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Linux (GCC/Clang, x64-linux):

```sh

cmake -S . -B build \
-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
-DVCPKG_MANIFEST_INSTALL=OFF \
-DCMAKE_PREFIX_PATH="/opt/Qt/6.8.0/gcc_64" \
-DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

macOS (AppleClang, x64-osx or arm64-osx):

```sh
cmake -S . -B build \
-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
-DVCPKG_MANIFEST_INSTALL=OFF \
-DCMAKE_PREFIX_PATH="~/Qt/6.8.0/macos" \
-DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Notes:
-

- If you want CMake to auto-install missing manifest dependencies, set:
  ```
    -DVCPKG_MANIFEST_INSTALL=ON
  ```
  
- If overlays are not picked automatically, add:
  ```
    -DVCPKG_OVERLAY_PORTS="%CD%\vcpkg-overlays"   # Windows
    -DVCPKG_OVERLAY_PORTS="$PWD/vcpkg-overlays"  # Linux/macOS
  ```

## Configure and build with a CMakeUserPresets under MSVC 2026 

For Windows under MSVC (2026), a CMakeUserPresets.json can also be used to make CMake building automatically with the MSVC. This file would include, as an example:

```sh
{
  "version": 2,
  "defaultConfigurePreset": "Release",
  "configurePresets": [
    {
      "name": "defaultBase",
      "inherits": "vcpkg",
      "binaryDir": "${sourceDir}/build",
	  "toolset": "v143",   // forces MSVC 19.44+  
      "environment": {
        "CMAKE_BUILD_TYPE": "Release",
        "VCPKG_ROOT": "C:\\Users\\fbas\\vcpkg"
      },

      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}\\scripts\\buildsystems\\vcpkg.cmake",
        "CMAKE_PREFIX_PATH": "C:/Qt6/6.8.3/msvc2022_64"
      },
	  "autoGenerate": true

    },
    {
      "name": "RelDebug",
      "inherits": "defaultBase",
      "binaryDir": "${sourceDir}/build/RelDebug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "RelWithDebInfo"
      }

    },
    {
      "name": "Release",
      "inherits": "defaultBase", 
      "binaryDir": "${sourceDir}/build/Release",

      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release"
      }

    },
    {
      "name": "Debug",
      "inherits": "defaultBase",
      "binaryDir": "${sourceDir}/build/Debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug"
      }

    }
  ]
}



    
```


## Using exported vcpkg dependencies

If you received dependencies as a vcpkg export (e.g., created with `vcpkg export --raw`), you can build without accessing vcpkg registries:

1. Extract the export archive somewhere. It should contain an `installed/` layout. The best location is `C:\vcpkg\`, so
that no changes are required to the vcpkg toolchain file.
NOTE: the root directory in the archive is named `vcpkg-export-something`, just rename it to `vcpkg`.

2. Point CMake/vcpkg to the exported “installed” directory:
   - Set the environment variable or CMake cache entry:
     ```
     -DVCPKG_INSTALLED_DIR="path/to/export/installed"
     -DVCPKG_FEATURE_FLAGS=manifests
     ```
   - Still pass the toolchain file:
     ```
     -DCMAKE_TOOLCHAIN_FILE=".../vcpkg.cmake"
     ```

3. Configure and build as usual. vcpkg will resolve packages from the exported “installed” tree and avoid network access.

If the export was produced as a NuGet package (`--nuget`), restore it into a local folder and pass `-DVCPKG_INSTALLED_DIR` to that restored layout.

## Tips

- Preferred triplets:
  - Windows: x64-windows
  - Linux: x64-linux
  - macOS: x64-osx (or arm64-osx on Apple Silicon)
- Ensure your Qt6 version matches the required 6.8.x indicated by CMake `find_package(Qt6 6.8 ...)`.
- For GDAL, if you hit API mismatches, use a version ≥ 2.20 where required by components that explicitly request it.
