// Icons.C

#include "Icons.H"
#include <QFile>

Icons::Icons() {
}

Icons::~Icons() {
}

QString Icons::fn(QString name) {
  return ":/icons/" + name + ".svg";
}

QString Icons::fn2(QString name) {
  return ":/icons/" + name + ".png";
}

bool Icons::contains(QString name) {
  if (data.contains(name))
    return true;
  QFile f(fn(name));
  if (f.exists())
    return true;
  QFile f2(fn2(name));
  if (f2.exists())
    return true;
  return false;
}

QIcon const &Icons::find(QString name) {
  if (data.contains(name))
    return data[name];

  QFile f(fn(name));
  if (f.exists()) 
    return data[name] = QIcon(fn(name));

  QFile f2(fn2(name));
  if (f2.exists()) 
    return data[name] = QIcon(fn2(name));

  return null;
}

Icons &Icons::icons() {
  static Icons ic;
  return ic;
}
