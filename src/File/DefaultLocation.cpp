// DefaultLocation.cpp

#include "DefaultLocation.h"
#include <QFileInfo>
QString _lastloc;

namespace DefaultLocation {
  QString lastLocation() {
    if (_lastloc.isEmpty())
      _lastloc = defaultLocation();
    return _lastloc;
  }

  void updateLastLocation(QString fn) {
    QFileInfo fi(fn);
    if (fi.isDir())
      _lastloc = fi.absolutePath();
    else
      _lastloc = fi.dir().absolutePath();
  }
  
  QString defaultLocation() {
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
    return res;
  }  

};
