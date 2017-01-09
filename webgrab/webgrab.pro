TEMPLATE = app
TARGET = ../build/webgrab
DEPENDPATH += .
INCLUDEPATH += .
QT += svg
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets printsupport
  linux {
    QT += webkit webkitwidgets
    QMAKE_CXXFLAGS += -DUSE_WEBKIT
  } else {
    QT += webengine webenginewidgets
    QMAKE_CXXFLAGS += -DUSE_WEBENGINE
  }
} else {
  QT += webkit
  QMAKE_CXXFLAGS += -DUSE_WEBKIT
}
CONFIG += debug_and_release
CONFIG-=app_bundle
CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

OBJECTS_DIR=../build/webgrab-release
CONFIG(debug, debug|release) { OBJECTS_DIR=../build/webgrab-debug }

# Input
HEADERS += Printer.h Options.h MozillaPage.h
SOURCES += Printer.cpp Options.cpp MozillaPage.cpp webgrab.cpp
