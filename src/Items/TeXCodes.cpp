// Items/TeXCodes.cpp - This file is part of eln

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

// TeXCodes.C

#include "TeXCodes.h"
#include "ElnAssert.h"
#include "JSONFile.h"

QMap<QString, QString> const &TeXCodes::chars() {
  static QMap<QString, QString> c;
  if (c.isEmpty()) {
    bool ok;
    QVariantMap v(JSONFile::load(":/TeXCodes.json", &ok));
    ASSERT(ok);
    foreach (QString k, v.keys())
      if (k != "#")
	c[k] = v[k].toString();
  }
  return c;
}

bool TeXCodes::contains(QString k) {
  return chars().contains(k);
}

QString TeXCodes::map(QString k) {
  if (contains(k))
    return chars()[k];
  else
    return "";
}
