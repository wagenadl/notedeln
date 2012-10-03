# -*- mode: shell-script; -*-

TEMPLATE = app
TARGET = eln
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lqjson
CONFIG += debug

HEADERS += \
    PageEditor.H \
    PageScene.H \
    Style.H
#    Data.H \
#    BlockData.H \
#    NoteData.H \
#    PageData.H \
#    PageFile.H 
SOURCES += \
    PageEditor.C \
    PageScene.C \
    Style.C \
    main.C
#    PageFile.C
