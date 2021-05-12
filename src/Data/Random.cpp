// Data/Random.cpp - This file is part of NotedELN

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

// Random.cpp

#include "Random.h"
#include <QDateTime>
#include <QHash>
#include <QNetworkInterface>

quint16 Random::random() {
  if (!inited())
    srandom();
  quint16 x = qrand();
  return x;
}

bool &Random::inited() {
  static bool x = false;
  return x;
}

void Random::srandom() {
  qint64 t = QDateTime::currentDateTime().toMSecsSinceEpoch();
  qsrand(t);
  foreach (QHostAddress a, QNetworkInterface::allAddresses())
    t ^= qHash(a);
  inited() = true;
}

    
