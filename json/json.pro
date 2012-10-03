# -*- mode: shell-script; -*-

TEMPLATE = app
TARGET = eln
DEPENDPATH += .
INCLUDEPATH += .
LIBRARIES += -lqjson
CONFIG += debug

HEADERS += \
    Data.H \
    BlockData.H \
    NoteData.H \
    PageData.H \
    PageFile.H 
SOURCES += \
    PageFile.C
