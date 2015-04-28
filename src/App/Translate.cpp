// Translate.cpp

#include "Translate.h"
#include "JSONFile.h"
#include <QDebug>

QString Translate::_(QString key) {
  if (map().contains(key))
    return map()[key];
  qDebug() << "No translation found for " << key;
  return key;
}

QString Translate::language() {
  return lang();
}

QString &Translate::lang() {
  static QString l;
  if (l.isEmpty()) {
    l = "-"; // avoid recursion
    setLanguage();
  }
  return l;
}

void Translate::setLanguage(QString l) {
  if (l.isEmpty()) {
    l = "en"; // should get from operating system
  }
  lang() = l;
}

QMap<QString, QString> const &Translate::map() {
  static QMap<QString, QString> mp;
  if (mp.isEmpty()) {
    mp["-"] = "-";
    bool ok;
    QVariantMap vm = JSONFile::load(":translate/" + language() + ".json", &ok);
    if (!ok)
      vm = JSONFile::load(":translate/en.json");
    foreach (QString k, vm.keys())
      mp[k] = vm[k].toString();
  }
  return mp;
}
