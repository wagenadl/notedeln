# Hey, emacs, show this as -*- mode: shell-script; -*-

QT += xml
CONFIG += debug
TEMPLATE = app
TARGET = 
DEPENDPATH += . 1stattempt
INCLUDEPATH += . 1stattempt

# Input
HEADERS += Data.H \
    DataBlock.H \
    DataBlockQuote.H \
    DataBlockText.H \
    DataGfx.H \
    DataNote.H \
    DataPage.H \
    DataText.H \
    FilePage.H

SOURCES += Data.C \
    DataBlock.C \
    DataBlockQuote.C \
    DataBlockText.C \
    DataGfx.C \
    DataNote.C \
    DataPage.C \
    DataText.C \
    FilePage.C \
    main.C