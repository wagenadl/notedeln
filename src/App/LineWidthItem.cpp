// App/LineWidthItem.cpp - This file is part of eln

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

// LineWidthItem.C

#include "LineWidthItem.h"
#include <QPainter>

LineWidthItem::LineWidthItem(double lw): lw(lw), c("black") {
}

LineWidthItem::~LineWidthItem() {
}

void LineWidthItem::paintContents(QPainter *p) {
  p->setPen(QPen(c, lw));
  p->setBrush(Qt::NoBrush);
  p->drawLine(12, 24, 20, 8); // or something like that
}

void LineWidthItem::setColor(QColor c1) {
  c = c1;
  update();
}


void LineWidthItem::setLineWidth(double lw1) {
  lw = lw1;
  update();
}

