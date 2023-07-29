// App/Version.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Version.cpp

#include "Version.h"
#include "config.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "BuildDate.h"

namespace Version {
  QDate buildDate() {
    QString date = ::buildDate();
    return QDate::fromString(date.simplified(), "MMM d yyyy");
  }

  QString toString() {
    static QString ver = "";
    if (ver.isEmpty()) {
      ver = QString::number(ELN_VERSION_MAJOR);
      ver += "." + QString::number(ELN_VERSION_MINOR);
      if (ELN_VERSION_PATCH & 1)
        // development version, include build date
        ver += "." + buildDate().toString("yyMMdd");
      else
        // release version, include patch number
        ver += "." + QString::number(ELN_VERSION_PATCH);
    }
    return ver;
  }
};

