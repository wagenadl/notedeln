TEMPLATE = app
TARGET = webgrab
DEPENDPATH += .
INCLUDEPATH += .
QT += webkit svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport webkitwidgets
CONFIG += debug_and_release
CONFIG-=app_bundle
CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

# Input
HEADERS += Printer.H Options.H MozillaPage.H
SOURCES += Printer.cpp Options.cpp MozillaPage.cpp webgrab.cpp
