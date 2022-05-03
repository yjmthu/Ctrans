@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if exist build (
    goto build
) else (
    mkdir build
    cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -S . -B build
)
:build
cd build
Ninja && start .\main.exe
exit 0
