// App/MarkSizeBar.cpp - This file is part of eln

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

// MarkSizeBar.C

#include "MarkSizeBar.h"
#include "MarkSizeItem.h"
#include "Mode.h"
#include <QDebug>

static double markSizes[] = {
  2,
  3,
  4.5,
  6.5,
  9,
  12.5,
};

static int nMarkSizes = sizeof(markSizes)/sizeof(*markSizes);

MarkSizeBar::MarkSizeBar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<nMarkSizes; i++) 
    addTool(sizeToId(markSizes[i]), new MarkSizeItem(markSizes[i]));

  mode->setMarkSize(markSizes[2]);
  select(sizeToId(mode->markSize()));
  setShape(mode->shape());
}

MarkSizeBar::~MarkSizeBar() {
}

void MarkSizeBar::setColor(QColor c) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setColor(c);
  }
}

void MarkSizeBar::setShape(GfxMarkData::Shape s) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setShape(s);
  }
}


void MarkSizeBar::doLeftClick(QString id, Qt::KeyboardModifiers) {
  mode->setMarkSize(idToSize(id));
}

double MarkSizeBar::idToSize(QString s) {
  return s.toDouble();
}

QString MarkSizeBar::sizeToId(double w) {
  return QString::number(w, 'f', 2);
}


