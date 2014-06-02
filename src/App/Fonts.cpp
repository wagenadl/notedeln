// Fonts.cpp

#include "Fonts.H"
#include <QFontDatabase>
#include <QStringList>
#include <QDebug>

Fonts::Fonts() {
    qDebug() << "FONTS";
  QStringList ff;
  ff << ":fonts/Ubuntu-BI.ttf";
  ff << ":fonts/Ubuntu-B.ttf";
  ff << ":fonts/Ubuntu-RI.ttf";
  ff << ":fonts/Ubuntu-R.ttf";

  foreach (QString f, ff)
    qDebug() << "adding font " << f << QFontDatabase::addApplicationFont(f);
}
