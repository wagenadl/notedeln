TEMPLATE = app
TARGET = webtopdf
DEPENDPATH += .
INCLUDEPATH += .
QT += webkit

# Input
HEADERS += Printer.H
SOURCES += webtopdf.C Printer.C
