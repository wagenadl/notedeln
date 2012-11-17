TEMPLATE = app
TARGET = webtosvg
DEPENDPATH += .
INCLUDEPATH += .
QT += webkit
QT += svg

# Input
HEADERS += Printer.H
SOURCES += webtosvg.C Printer.C
