// Items/Digraphs.cpp - This file is part of NotedELN

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

// Digraphs.C

#include "Digraphs.h"

#include "ElnAssert.h"
#include "JSONFile.h"

QMap<QString, QMap<QString, QString> > const &Digraphs::chars() {
  static QMap<QString, QMap<QString, QString> > c;
  if (c.isEmpty()) {
    bool ok;
    QVariantMap v(JSONFile::load(":/Digraphs.json", &ok));
    ASSERT(ok);
    foreach (QString k, v.keys())
      if (k != "#")
	c[k.right(1)][k] = v[k].toString();
  }
  return c;
}

bool Digraphs::anyEndingWith(QString k) {
  return chars().contains(k);
}

QMap<QString, QString> const &Digraphs::allEndingWith(QString k) {
  ASSERT(anyEndingWith(k));
  return chars().find(k).value();
}

bool Digraphs::contains(QString k) {
  return anyEndingWith(k.right(1))
    ? allEndingWith(k.right(1)).contains(k)
    : false;
}

QString Digraphs::map(QString k) {
  if (contains(k))
    return allEndingWith(k.right(1))[k];
  else
    return "";
}
