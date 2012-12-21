# QMake project file for eln                  -*- mode: shell-script; -*-
#
# When adding source files, run scripts/updatesources to include them

TEMPLATE = app
TARGET = eln
LIBS += -lqjson

include(eln.pri)

# Nothing below this line is particularly interesting

CONFIG += debug
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc
QT += network svg

DEPENDPATH +=  $$sourcedirs
INCLUDEPATH += $$sourcedirs

unix: DEFINES += QQ_X11

for(sd, sourcedirs): include($${sd}/$${sd}.pri)

message("HEADERS: $$HEADERS" )
message("SOURCES: $$SOURCES" )
message("RESOURCES: $$RESOURCES" )
message("INCLUDEPATH: $$sourcedirs" )
message("TARGET: $$TARGET")
