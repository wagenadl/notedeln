// WebGrab.cpp - part of ELN

#include "WebGrab.H"
#include <QApplication>

QString WebGrab::executable() {
  return QApplication::applicationDirPath() + "/webgrab";
}

 
