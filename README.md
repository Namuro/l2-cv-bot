## WIP

Simple Lineage II bot that uses CV to find possible targets and monitor HP/MP/CP.

![Preview](preview.png)

## Features

* NPC detection
* HP/MP/CP monitoring
* ~~Mouse & keyboard emulation~~
* ~~Configs for various clients~~
* ~~Stuck resolving~~
* ~~Captcha alarm~~
* ~~Multi-monitor support~~

## How to use

1. Run Lineage II client, select your character and teleport to any exp/farm location
2. Run `l2-cv-bot.exe --window <title of the Lineage II client window> --debug true`
3. Your HP/CP/MP bars must be 100% at the moment of bot start, but if not, you should wait when they will be 100% and then press Space to reset bars positions
4. To stop press ESC or just move mouse (if bot not running in debug mode) 

*Note that ESC and Space key bindings may not work in Lineage II window, so you just need to select any other window/desktop and then press keys.*

## Command line options

```
--window    Lineage II window title or part of the title. Default: "Lineage II"
--debug     Show window with debug information. Press ESC to exit or Space to reset bar positions. Default: true
```

## Customization

Current version developed and tested using Windows 10 and Gracia Epilogue client, so with another Windows or Lineage II client it may not work.

To customize for your client, you need to edit properties in `Eyes.h` file and rebuild project. Note that for colors you must use HSV color model.

OS related stuff placed in these files: `Window.cpp`, `Capture.cpp`, `Input.cpp`.

## Building

Using CMake & MSVC (Visual Studio 2017) on Windows:

1. [Download](https://cmake.org/download/) and install CMake
2. [Download](https://opencv.org/releases.html) OpenCV binaries or [build](https://github.com/opencv/opencv) OpenCV manually
3. Set `OpenCV_DIR` environment variable to path of the OpenCV binaries
4. Run `build-vs15x64.bat` for 64-bit OpenCV or `build-vs15x32.bat` for 32-bit OpenCV (or `build.bat` to provide custom arch, project generator and configuration)
5. Built executable and dependent dynamic libraries can be found in `build-<arch>/<configuration>` directory (e.g. `build-x64/Release`)
