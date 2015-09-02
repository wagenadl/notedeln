// File/RmDir.cpp - This file is part of eln

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

// RmDir.cpp

#include "RmDir.h"
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>

namespace RmDir {
  static void recurseNoCheck(QDir dir) {
    // this is *VERY* dangerous
    QFileInfoList infos =  dir.entryInfoList(QDir::NoDotAndDotDot 
                                             | QDir::AllDirs | QDir::Files
                                             | QDir::Hidden
                                             | QDir::System);
    foreach (QFileInfo info, infos) {
      if (info.isDir()) {
        recurseNoCheck(info.absoluteFilePath());
      } else {
        QFile::remove(info.absoluteFilePath());
      }
    }
    QDir::current().rmdir(dir.absolutePath());
  }

  bool recurse(QString dir) {
    if (!dir.endsWith(".nb")) {
      return false;
    }
    QDir d(dir);
    if (!d.exists()) {
      return false;
    }

#if QT_VERSION >=0x050000
    return d.removeRecursively();
#else
    recurseNoCheck(d);
    QDir dd(dir);
    if (dd.exists()) {
      return false;
    }
    return true;
#endif
  }
}
