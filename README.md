## WIP

Simple Lineage II bot that uses CV to find possible targets and monitor HP/MP/CP.

![Preview](preview.png)

## Features

* NPC detection
* HP/MP/CP monitoring
* ~~Mouse & keyboard emulation~~
* ~~Runtime config per client support~~
* ~~Stuck resolving~~
* ~~Captcha alarm~~
* ~~Multi-monitor support~~

## Command line options

```
--window    Lineage II window title or part of the title. Default: "Lineage II"
--debug     Show window with debug information. Press ESC to exit or Space to reset bar positions. Default: true
```

## Customization

Current version developed and tested using Windows 10 and Gracia Epilogue client, so with on another Windows or Lineage II client it might not work.

To customize for your client, you need to edit properties in `Eyes.h` file and build project. Note that for colors you must use HSV color model.

## Building

Using CMake & MSVC (Visual Studio 2017) on Windows:

1. [Download](https://cmake.org/download/) and install CMake
2. [Download](https://opencv.org/releases.html) OpenCV binaries or [build](https://github.com/opencv/opencv) OpenCV manually
3. Set `OpenCV_DIR` environment variable to path of the OpenCV binaries
4. Run `build-vs15x64.bat` for 64-bit OpenCV or `build-vs15x32.bat` for 32-bit OpenCV (or `build.bat` to provide custom arch, project generator and configuration)
5. Built executable and dependent dynamic libraries can be found in `build-<arch>/<configuration>` directory (e.g. `build-x64/Release`)
