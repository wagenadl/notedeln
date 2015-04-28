// WebGrab.cpp - part of ELN

#include "WebGrab.h"
#include <QApplication>

QString WebGrab::executable() {
  return QApplication::applicationDirPath() + "/webgrab";
}

 
