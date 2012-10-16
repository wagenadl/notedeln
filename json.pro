# -*- mode: shell-script; -*-

TEMPLATE = app
TARGET = eln
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lqjson
CONFIG += debug
MOC_DIR = .moc
OBJECTS_DIR = .obj
QT += network

RESOURCES += icons.qrc

HEADERS += \
    Data.H \
    MarkupData.H \
    TextData.H \
    TitleData.H \
    GfxData.H \
    GfxPointsData.H \
    GfxTextData.H \
    GfxImageData.H \
    BlockData.H \
    TextBlockData.H \
    GfxBlockData.H \
    NoteData.H \
    PageData.H \
    JSONFile.H \
    DataFile.H \
    PageFile.H \
    Icons.H \
    PageEditor.H \
    PageScene.H \
    TitleItem.H \
    BlockItem.H \
    GfxBlockItem.H \
    GfxImageItem.H \
    GfxItemFactory.H \
    TextBlockItem.H \
    TextItem.H \
    TitleTextItem.H \
    TextMarkings.H \
    PageView.H \
    Style.H \
    Toolbar.H \
    ResourceLoader.H \
    ResourceManager.H \
    Notebook.H \
    TOC.H \
    BookData.H \
    BookFile.H

SOURCES += \
    Data.C \
    MarkupData.C \
    TextData.C \
    TitleData.C \
    GfxData.C \
    GfxPointsData.C \
    GfxTextData.C \
    GfxImageData.C \
    BlockData.C \
    TextBlockData.C \
    GfxBlockData.C \
    NoteData.C \
    PageData.C \
    JSONFile.C \
    DataFile.C \
    Icons.C \
    main.C \
    PageEditor.C \
    PageScene.C \
    TitleItem.C \
    BlockItem.C \
    GfxBlockItem.C \
    GfxImageItem.C \
    GfxItemFactory.C \
    TextBlockItem.C \
    TextItem.C \
    TitleTextItem.C \
    TextMarkings.C \
    PageView.C \
    Style.C \
    Toolbar.C \
    ResourceLoader.C \
    ResourceManager.C \
    Notebook.C \
    TOC.C \
    BookData.C

	
