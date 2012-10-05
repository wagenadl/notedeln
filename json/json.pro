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
    Data.H \
    BlockData.H \
    TextBlockData.H \
    GfxBlockData.H \
    GfxData.H \
    GfxPointsData.H \
    GfxTextData.H \
    MarkupData.H \
    NoteData.H \
    PageData.H \
    PageFile.H \
    Icons.H \
    PageEditor.H \
    PageScene.H \
    BlockItem.H \
    TextBlockItem.H \
    TextBlockTextItem.H \
    PageView.H \
    Style.H \
    Toolbar.H

SOURCES += \
    Data.C \
    BlockData.C \
    TextBlockData.C \
    GfxBlockData.C \
    GfxData.C \
    GfxPointsData.C \
    GfxTextData.C \
    MarkupData.C \
    NoteData.C \
    PageData.C \
    PageFile.C \
    Icons.C \
    main.C \
    PageEditor.C \
    PageScene.C \
    BlockItem.C \
    TextBlockItem.C \
    TextBlockTextItem.C \
    PageView.C \
    Style.C \
    Toolbar.C

	
