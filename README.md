# WIP

Simple Lineage II bot that uses CV to find possible targets and monitor HP/MP/CP.

![Preview](preview.png)

## Features

* NPC detection
* HP/MP/CP monitoring
* Custom CV configuration support
* ~~Mouse and keyboard emulation~~
* ~~Custom skills/actions layout support~~
* ~~Stuck resolving~~
* ~~Alarm subsystem (captcha, low HP, CP decreasing, etc.)~~
* ~~Buff/debuff monitoring~~
* ~~Multi-monitor support~~

## How to use

**WARNING: This bot requires [Interception](https://github.com/oblitum/Interception) kernel mode driver for mouse and keyboard emulation.**

**It's a closed-source kernel mode driver, so we need to completely trust the author, even if a driver signed by Microsoft. The driver allows input to be monitored, as well as emulated, so theoretically any unprivileged software could use it to sniff what you're typing. It can also block any keyboard input, including Ctrl+Alt+Del.**

Install [Interception](https://github.com/oblitum/Interception/releases) driver (start `cmd.exe` as Administrator, then run `install-interception.exe /install`) and reboot.

1. Run Lineage II client, select character and teleport to any exp/farm location
2. Run `run.bat "<title of the Lineage II client window>"`
3. Character's HP/CP/MP bars must be 100% at the moment of bot start, but if not, you should wait until they will be 100% and then press Space to reset bars position
4. To stop press ESC or just move mouse (if not running in debug mode)

## Command line options

`l2-cv-bot.exe`:

```
--window    Lineage II window title or part of the title. Default: "Lineage II"
--debug     Show window with debug information. Press ESC to exit or Space to reset bars position. Default: true
```

`run.bat` only accepts window title.

## Customization

Current version developed and tested using Windows 10 and Gracia Epilogue client, so with another Windows or Lineage II client it may not work.

Edit `run.bat` file to customize for another client. Note that colors must use HSV color model.

OS related stuff placed in these files: `Window.cpp`, `Capture.cpp`, `Input.cpp`.

## Building

Using CMake and MSVC (Visual Studio 2017) on Windows:

1. [Download](https://cmake.org/download/) and install CMake
2. [Download](https://opencv.org/releases.html) OpenCV binaries or [build](https://github.com/opencv/opencv) OpenCV manually
3. Set `OpenCV_DIR` environment variable to path of the OpenCV binaries
4. Run `build-vs15x64.bat` for 64-bit OpenCV or `build-vs15x32.bat` for 32-bit OpenCV (or `build.bat` to provide custom arch, project generator and configuration)
5. Built executable and dependent dynamic libraries can be found in `build-<arch>/<configuration>` directory (e.g. `build-x64/Release`)

*If Interception library building fails, try to install [WDK](https://www.microsoft.com/en-us/download/details.aspx?id=11800).*