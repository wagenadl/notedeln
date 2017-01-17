TEMPLATE = app
TARGET = ../build/webgrab
DEPENDPATH += .
INCLUDEPATH += .
QT += svg
win32 {
  message("win32")
  CONFIG += console
}
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets printsupport
  linux {
    QT += webkit webkitwidgets
    QMAKE_CXXFLAGS += -DUSE_WEBKIT
    HEADERS += Printer.h
    SOURCES += webgrab.cpp MozillaPage.cpp Printer.cpp
  } else {
    QT += webengine webenginewidgets
    QMAKE_CXXFLAGS += -DUSE_WEBENGINE
    HEADERS += PrinterWE.h
    SOURCES += webgrabWE.cpp PrinterWE.cpp
  }
} else {
  QT += webkit
  QMAKE_CXXFLAGS += -DUSE_WEBKIT
  HEADERS += Printer.h
  SOURCES += webgrab.cpp MozillaPage.cpp Printer.cpp
}
  
CONFIG += debug_and_release
CONFIG-=app_bundle
CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

OBJECTS_DIR=../build/webgrab-release
CONFIG(debug, debug|release) { OBJECTS_DIR=../build/webgrab-debug }
MOC_DIR = $${OBJECTS_DIR}
RCC_DIR = $${OBJECTS_DIR}
UI_DIR = $${OBJECTS_DIR}
          
# Input
HEADERS += Options.h
SOURCES += Options.cpp
