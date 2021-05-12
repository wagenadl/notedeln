// Items/TeXCodes.cpp - This file is part of NotedELN

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

// TeXCodes.C

#include "TeXCodes.h"
#include "ElnAssert.h"
#include "JSONFile.h"

QMap<QString, QString> TeXCodes::nox_;
QMap<QString, QString> TeXCodes::map_;

void TeXCodes::ensure() {
  if (!map_.isEmpty())
    return;

  bool ok;
  QVariantMap kv(JSONFile::load(":/TeXCodes.json", &ok));
  ASSERT(ok);

  foreach (QString k, kv.keys()) {
    if (k.startsWith("#"))
      continue;
    QString v = kv[k].toString();
    if (k.startsWith("*")) {
      k = k.mid(1);
      nox_[k] = v;
    }
    map_[k] = v;
  }
}

bool TeXCodes::contains(QString k) {
  ensure();
  return map_.contains(k);
}

QString TeXCodes::map(QString k) {
  if (contains(k))
    return map_[k];
  else
    return "";
}

bool TeXCodes::onlyExplicit(QString k) {
  ensure();
  return k.size()==1 || nox_.contains(k);
}
