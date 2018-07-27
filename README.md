## WIP

Simple L2 bot that uses CV to find possible targets and monitor HP/MP/CP.

### Features:

* ~~NPC detection~~
* ~~HP/MP/CP monitoring~~
* ~~Mouse & keyboard emulation~~
* ~~Captcha alarm~~
* ~~Multi-monitor support~~

### Building on Windows using CMake & MSVC (Visual Studio 2017):

1. [Download](https://cmake.org/download/) and install CMake
2. [Download](https://opencv.org/releases.html) OpenCV binaries or [build](https://github.com/opencv/opencv) OpenCV manually
3. Set `OpenCV_DIR` environment variable to path of the OpenCV binaries
4. Run `build-vs15x64.bat` for 64-bit OpenCV or `build-vs15x32.bat` for 32-bit OpenCV (or `build.bat` to provide custom arch, project generator and configuration)
5. Built executable and dependent dynamic libraries can be found in `build-<arch>/<configuration>` directory (e.g. `build-x32/Release`)
