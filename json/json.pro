# -*- mode: shell-script; -*-

TEMPLATE = app
TARGET = eln
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lqjson
CONFIG += debug
MOC_DIR = .moc
OBJECTS_DIR = .obj

RESOURCES += icons.qrc

HEADERS += \
    BlockData.H \
    Data.H \
    GfxBlockData.H \
    GfxData.H \
    GfxPointsData.H \
    GfxTextData.H \
    Icons.H \
    MarkupData.H \
    NoteData.H \
    PageData.H \
    PageEditor.H \
    PageFile.H \
    PageScene.H \
    PageView.H \
    Style.H \
    TextBlockData.H \
    Toolbar.H

SOURCES += \
    BlockData.C \
    Data.C \
    GfxBlockData.C \
    GfxData.C \
    GfxPointsData.C \
    GfxTextData.C \
    Icons.C \
    main.C \
    MarkupData.C \
    NoteData.C \
    PageData.C \
    PageEditor.C \
    PageFile.C \
    PageScene.C \
    PageView.C \
    Style.C \
    TextBlockData.C \
    Toolbar.C

	
