// Items/Accents.cpp - This file is part of NotedELN

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

// Accents.C

#include "Accents.h"
#include "ElnAssert.h"
#include "JSONFile.h"

QMap<QString, QString> const &Accents::chars() {
  static QMap<QString, QString> c;
  if (c.isEmpty()) {
    bool ok;
    QVariantMap v(JSONFile::load(":/Accents.json", &ok));
    ASSERT(ok);
    foreach (QString k, v.keys())
      if (k != "#")
	c[k] = v[k].toString();
  }
  return c;
}

bool Accents::contains(QString k) {
  return chars().contains(k);
}

QString Accents::map(QString k) {
  if (contains(k))
    return chars()[k];
  else
    return "";
}
