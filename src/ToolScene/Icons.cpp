// App/Icons.cpp - This file is part of NotedELN

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

// Icons.C

#include "Icons.h"
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
