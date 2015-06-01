#!/bin/sh

cd build-eln-Desktop_Qt_5_2_0_MinGW_32bit-Release/release
PATH=/cygdrive/c/Qt/5.2.0/mingw48_32/bin:$PATH
windeployqt.exe eln.exe
