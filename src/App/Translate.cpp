// App/Translate.cpp - This file is part of NotedELN

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
