# QMake project file for eln                  -*- mode: shell-script; -*-
#
# When adding source files, run scripts/updatesources to include them

TEMPLATE = app
TARGET = repairtoc

SOURCES = repairtoc.cpp
SOURCES += JSONFile.cpp
HEADERS += JSONFile.H
SOURCES += JSONParser.cpp
HEADERS += JSONParser.H

CONFIG += debug_and_release
CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }
