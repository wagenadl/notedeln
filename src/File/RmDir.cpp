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
