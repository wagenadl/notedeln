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
    MarkupData.H \
    TextData.H \
    TitleData.H \
    GfxData.H \
    GfxPointsData.H \
    GfxTextData.H \
    BlockData.H \
    TextBlockData.H \
    GfxBlockData.H \
    NoteData.H \
    PageData.H \
    PageFile.H \
    Icons.H \
    PageEditor.H \
    PageScene.H \
    TitleItem.H \
    BlockItem.H \
    TextBlockItem.H \
    TextItem.H \
    TitleTextItem.H \
    TextMarkings.H \
    PageView.H \
    Style.H \
    Toolbar.H

SOURCES += \
    Data.C \
    MarkupData.C \
    TextData.C \
    TitleData.C \
    GfxData.C \
    GfxPointsData.C \
    GfxTextData.C \
    BlockData.C \
    TextBlockData.C \
    GfxBlockData.C \
    NoteData.C \
    PageData.C \
    PageFile.C \
    Icons.C \
    main.C \
    PageEditor.C \
    PageScene.C \
    TitleItem.C \
    BlockItem.C \
    TextBlockItem.C \
    TextItem.C \
    TitleTextItem.C \
    TextMarkings.C \
    PageView.C \
    Style.C \
    Toolbar.C

	
