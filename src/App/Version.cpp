// App/Version.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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

    
  QDateTime buildDate() {
    QString date = QString(__DATE__);
    QString time = QString(__TIME__);
    QDate d(QDate::fromString(date.simplified(), "MMM d yyyy"));
    QTime t(QTime::fromString(time));
    return QDateTime(d, t);
  }

  QString toString() {
    static QString ver = "";
    if (ver.isEmpty()) {
      ver = verbit();
      int idx = ver.lastIndexOf(".");
      QString subv = ver.mid(idx+1);
      // if (subv=="?" || (subv.toInt()&1)==1) {
      //   // odd version: development
      //   QDateTime dt = buildDate();
      //   ver += "." + dt.toString("yyMMdd");
      // }
    }
    return ver;
  }
};

