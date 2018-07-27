@echo off
cmake -H. -B%1 -G %2
cmake --build %1 --config %3 --target INSTALL
echo.
pause
