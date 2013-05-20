// App/MarkSizeItem.cpp - This file is part of eln

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

// MarkSizeItem.C

#include "MarkSizeItem.H"
#include <QPainter>
#include "GfxMarkItem.H"

MarkSizeItem::MarkSizeItem(double ms):
  ms(ms), c("black"), shp(GfxMarkData::SolidCircle) {
}

MarkSizeItem::~MarkSizeItem() {
}

void MarkSizeItem::paintContents(QPainter *p) {
  GfxMarkItem::renderMark(QPointF(16, 16), c, ms, shp, p);
}

void MarkSizeItem::setColor(QColor c1) {
  c = c1;
  update();
}

void MarkSizeItem::setShape(GfxMarkData::Shape s1) {
  shp = s1;
  update();
}

void MarkSizeItem::setMarkSize(double s1) {
  ms = s1;
  update();
}

