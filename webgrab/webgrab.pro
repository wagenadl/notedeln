TEMPLATE = app
TARGET = webgrab
DEPENDPATH += .
INCLUDEPATH += .
QT += webkit
QT += svg
CONFIG += debug

# Input
HEADERS += Printer.H Options.H MozillaPage.H
SOURCES += Printer.C Options.C MozillaPage.C webgrab.C
