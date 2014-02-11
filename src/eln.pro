# QMake project file for eln                  -*- mode: shell-script; -*-
#
# When adding source files, run scripts/updatesources to include them

TEMPLATE = app
TARGET = eln
#LIBS += -lqjson

include(eln.pri)

# Nothing below this line is particularly interesting

CONFIG += debug_and_release
QT += network svg webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

DEPENDPATH +=  $$sourcedirs
INCLUDEPATH += $$sourcedirs

win: RC_FILE = App/winicon.rc
mac {
    ICON = App/eln.iconset
    QMAKE_INFO_PLIST = App/Info.plist
    OTHER_FILES += Info.plist
    }
# acdeployqt myapplication.app
# hdiutil create -format UDBZ -quiet -srcfolder myapplication.app myapplication.dmg 

for(sd, sourcedirs): include($${sd}/$${sd}.pri)

CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

message("HEADERS: $$HEADERS" )
message("SOURCES: $$SOURCES" )
message("RESOURCES: $$RESOURCES" )
message("INCLUDEPATH: $$sourcedirs" )
message("TARGET: $$TARGET")
