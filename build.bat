@echo off
cmake -H. -B%1 -G %2
cmake --build %1 --config Release --target INSTALL
echo.
pause
