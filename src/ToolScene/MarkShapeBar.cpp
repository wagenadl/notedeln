// App/MarkShapeBar.cpp - This file is part of eln

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

// MarkShapeBar.C

#include "MarkShapeBar.h"
#include "MarkSizeItem.h"
#include "Mode.h"
#include <QDebug>

MarkShapeBar::MarkShapeBar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<=GfxMarkData::LAST; i++) {
    GfxMarkData::Shape s = (GfxMarkData::Shape)(i);
    MarkSizeItem *it = new MarkSizeItem(mode->markSize());
    it->setShape(s);
    addTool(shapeToId(s), it);
  }
  mode->setShape(GfxMarkData::SolidCircle);
  select(shapeToId(mode->shape()));
  setColor(mode->color());
}

MarkShapeBar::~MarkShapeBar() {
}

void MarkShapeBar::setColor(QColor c) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setColor(c);
  }
}

void MarkShapeBar::setMarkSize(double s) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setMarkSize(s);
  }
}
 
void MarkShapeBar::doLeftClick(QString id, Qt::KeyboardModifiers) {
  mode->setShape(idToShape(id));
}

GfxMarkData::Shape MarkShapeBar::idToShape(QString s) {
  return (GfxMarkData::Shape)(s.toInt());
}

QString MarkShapeBar::shapeToId(GfxMarkData::Shape s) {
  return QString::number(int(s));
}


