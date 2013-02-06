# QMake project file for eln                  -*- mode: shell-script; -*-
#
# When adding source files, run scripts/updatesources to include them

TEMPLATE = app
TARGET = repairtoc
LIBS += -lqjson

SOURCES = repairtoc.C
SOURCES += JSONFile.C
HEADERS += JSONFile.H

CONFIG += debug
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc
