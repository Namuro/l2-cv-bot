@echo off
:ask
set /P INPUT=Delete build directories? (Y/N): %=%
if /I "%INPUT%"=="y" goto yes 
if /I "%INPUT%"=="n" goto no
goto ask
:yes
for /D %%f in (build*) do @rmdir "%%f" /Q /S
:no
