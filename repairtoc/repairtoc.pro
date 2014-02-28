# QMake project file for eln                  -*- mode: shell-script; -*-
#
# When adding source files, run scripts/updatesources to include them

TEMPLATE = app
TARGET = eln-repairtoc

SOURCES = repairtoc.cpp
SOURCES += ../src/File/JSONFile.cpp
HEADERS += ../src/File/JSONFile.H
SOURCES += ../src/File/JSONParser.cpp
HEADERS += ../src/File/JSONParser.H
INCLUDEPATH += ../src/File

CONFIG += debug_and_release
CONFIG -= app_bundle
CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }
