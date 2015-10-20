// DefaultLocation.h

#ifndef DEFAULTLOCATION_H

#define DEFAULTLOCATION_H

#include <QDir>
#include <QDebug>

inline QString defaultLocation() {
  QDir loc(QDir::home());
  if (loc.exists("Documents"))
    loc.cd("Documents");
  else if (loc.exists("My Documents"))
    loc.cd("My Documents");
  if (loc.exists("notebooks"))
    loc.cd("notebooks");
  else if (loc.exists("Notebooks"))
    loc.cd("Notebooks");
  QString res = loc.exists() ? loc.absolutePath() : QDir::homePath();
  qDebug() << "defaultLocation" << res;
  return res;
}  

#endif

