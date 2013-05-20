TEMPLATE = app
TARGET = webgrab
DEPENDPATH += .
INCLUDEPATH += .
QT += webkit
QT += svg
CONFIG += debug
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc

# Input
HEADERS += Printer.H Options.H MozillaPage.H
SOURCES += Printer.cpp Options.cpp MozillaPage.cpp webgrab.cpp
