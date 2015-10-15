// App/Cursors.cpp - This file is part of eln

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

// Cursors.cpp

#include "Cursors.h"
#include <QPixmap>

QPixmap &Cursors::crossPixmap() {
  static QPixmap ch(":icons/crosshair.png");
  return ch;
}


QCursor Cursors::crossCursor() {
  return QCursor(crossPixmap(), 16, 16);
}

QCursor Cursors::refined(Qt::CursorShape cs) {
  if (cs==Qt::CrossCursor)
    return crossCursor();
  else
    return QCursor(cs);
}

