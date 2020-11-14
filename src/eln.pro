# QMake project file for eln                  -*- mode: shell-script; -*-

# When adding source files, run scripts/updatesources to include them

TEMPLATE = app
TARGET = eln
#LIBS += -lqjson

include(eln.pri)

# Nothing below this line is particularly interesting

CONFIG += debug_and_release
QT += network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

DEPENDPATH +=  $$sourcedirs
INCLUDEPATH += $$sourcedirs

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += c++11
} else {
    QMAKE_CXXFLAGS += -std=c++11
}

linux {
  QMAKE_CXXFLAGS += -Wall -Wextra
}

win32 {
    RC_FILE = App/winicon.rc
    LIBS += -lSecur32
}

mac {
    ICON = App/eln.icns
    #QMAKE_MAC_SDK = macosx10.11
    QMAKE_MAC_SDK = macosx
    QMAKE_INFO_PLIST = App/Info.plist
    OTHER_FILES += App/Info.plist
    TARGET = ../eln
    }

# macdeployqt myapplication.app
# hdiutil create -format UDBZ -quiet -srcfolder myapplication.app myapplication.dmg 

for(sd, sourcedirs): include($${sd}/$${sd}.pri)

win32 {
} else {
  RESOURCES -= App/conf.qrc
}

CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

tgt_ver.target = ../build/version_stub.h
tgt_ver.commands = touch ../build/version_stub.h
tgt_ver.depends = tgt_v2
tgt_v2.commands =
#QMAKE_EXTRA_TARGETS += tgt_ver tgt_v2
#PRE_TARGETDEPS = ../build/version_stub.h
