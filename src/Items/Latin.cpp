// Items/Latin.cpp - This file is part of NotedELN

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

// Latin.C

#include "Latin.h"
#include "ElnAssert.h"
#include "JSONFile.h"

QVariantMap const &Latin::json() {
  static QVariantMap vv;
  if (vv.isEmpty()) {
    bool ok = false;
    vv = JSONFile::load(":/Latin.json", &ok);
    ASSERT(ok);
  }
  return vv;
}
  
QMap<QString, QSet<QString> > Latin::load(QString lbl) {
  QMap<QString, QSet<QString> > c;
  ASSERT(json().contains(lbl));
  QVariantMap const &map = json()[lbl].toMap();
  foreach (QString k, map.keys()) {
    QSet<QString> vv;
    QVariantList lst = map[k].toList();
    foreach (QVariant v, lst)
      vv << v.toString();
    c[k] = vv;
  }      
  return c;
}

QMap<QString, QSet<QString> > const &Latin::normal() {
  static QMap<QString, QSet<QString> > c;
  if (c.isEmpty()) 
    c = load("normal");
  return c;
}

QSet<QString> const &Latin::normal(QString s) {
  auto map = normal();
  if (map.contains(s))
    return map[s];
  static QSet<QString> empty;
  return empty;
}

QMap<QString, QSet<QString> > const &Latin::abbrev() {
  static QMap<QString, QSet<QString> > c;
  if (c.isEmpty()) 
    c = load("abbrev");
  return c;
}

QSet<QString> const &Latin::abbrev(QString s) {
  auto map = abbrev();
  if (map.contains(s))
    return map[s];
  static QSet<QString> empty;
  return empty;
}

