// Version.cpp

#include "Version.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

namespace Version {
  QString verbit() {
    static QString ver = "";
    if (ver.isEmpty()) {
      QFile fv(":/version");
      if (fv.open(QFile::ReadOnly)) 
	ver = QString(fv.readAll()).simplified();
      else
	ver = "?.?.?";
    }
    return ver;
  }

  QString patchbit() {
    static QString ver = "";
    if (ver.isEmpty()) {
      QFile fv(":/patchlevel");
      if (fv.open(QFile::ReadOnly)) 
	ver = QString(fv.readAll()).simplified();
      else
	ver = "";
    }
    return ver;
  }
    
  QString toString() {
    QString p = patchbit();
    QString v = verbit();
    return p.isEmpty() ? v : v + "+" + p;
  }

  QDateTime buildDate() {
    QString date = QString(__DATE__);
    QString time = QString(__TIME__);
    qDebug() << date << time;
    QDate d(QDate::fromString(date.simplified(), "MMM d yyyy"));
    QTime t(QTime::fromString(time));
    qDebug() << d << t;
    return QDateTime(d, t);
  }

};

