@echo off
set PATH=D:\QT\Tools\mingw1310_64\bin;D:\QT\Tools\CMake_64\bin;%PATH%
cd /d X:\SHOPNOVA2\build_nospaces
cmake.exe -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="g++.exe" -DCMAKE_MAKE_PROGRAM="mingw32-make.exe" -DCMAKE_PREFIX_PATH="D:\QT\6.11.0\mingw_64" ..
cmake.exe --build . --target all
