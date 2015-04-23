// Fonts.cpp

#include "Fonts.h"
#include <QFontDatabase>
#include <QStringList>
#include <QDebug>

Fonts::Fonts() {
  QStringList ff;
  ff << ":fonts/Ubuntu-BI.ttf";
  ff << ":fonts/Ubuntu-B.ttf";
  ff << ":fonts/Ubuntu-RI.ttf";
  ff << ":fonts/Ubuntu-R.ttf";

  foreach (QString f, ff)
    QFontDatabase::addApplicationFont(f);
}
