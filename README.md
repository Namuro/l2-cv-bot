## WIP

Simple Lineage II bot that uses CV to find possible targets and monitor HP/MP/CP.

![Preview](preview.png)

## Features:

* NPC detection
* HP/MP/CP monitoring
* ~~Mouse & keyboard emulation~~
* ~~Stuck resolving~~
* ~~Captcha alarm~~
* ~~Multi-monitor support~~

## Command line options

```
--window    Lineage II window title or part of the title. Default: "Lineage 2"
--debug     Show window with debug information & print more debug messages in console.
```

## Building:

Using CMake & MSVC (Visual Studio 2017) on Windows:

1. [Download](https://cmake.org/download/) and install CMake
2. [Download](https://opencv.org/releases.html) OpenCV binaries or [build](https://github.com/opencv/opencv) OpenCV manually
3. Set `OpenCV_DIR` environment variable to path of the OpenCV binaries
4. Run `build-vs15x64.bat` for 64-bit OpenCV or `build-vs15x32.bat` for 32-bit OpenCV (or `build.bat` to provide custom arch, project generator and configuration)
5. Built executable and dependent dynamic libraries can be found in `build-<arch>/<configuration>` directory (e.g. `build-x64/Release`)
