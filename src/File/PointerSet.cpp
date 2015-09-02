// File/PointerSet.cpp - This file is part of eln

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

// PointerSet.cpp

#include "PointerSet.h"

void PointerSet::insert(QObject *o) {
  data.insert(o);
  connect(o, SIGNAL(destroyed(QObject*)), SLOT(remove(QObject*)));
}

void PointerSet::remove(QObject *o) {
  if (data.remove(o) && isEmpty())
    emit emptied();
}

bool PointerSet::isEmpty() const {
  return data.isEmpty();
}

void PointerSet::clear() {
  if (isEmpty())
    return;
  data.clear();
  emit emptied();
}
