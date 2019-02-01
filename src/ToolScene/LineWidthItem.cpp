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
#include <math.h>

LineWidthItem::LineWidthItem(double lw): lw(lw), c("black") {
  straight = false;
}

LineWidthItem::~LineWidthItem() {
}

void LineWidthItem::setStraightLineMode(bool s) {
  straight = s;
  update();
}

void LineWidthItem::paintContents(QPainter *p) {
  static QPolygonF pp;
  if (pp.isEmpty()) 
    for (double x=10; x<22.1; x+=0.5) 
      pp << QPointF(x, 16-(x-16)-sin(.7*(x-12)));
  p->setPen(QPen(c, lw));
  p->setBrush(Qt::NoBrush);
  if (straight)
    p->drawLine(QLineF(QPointF(10, 22), QPointF(22, 10)));
  else
    p->drawPolyline(pp);
}

void LineWidthItem::setColor(QColor c1) {
  c = c1;
  update();
}


void LineWidthItem::setLineWidth(double lw1) {
  lw = lw1;
  update();
}

