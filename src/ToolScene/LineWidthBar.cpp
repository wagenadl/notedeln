// App/LineWidthBar.cpp - This file is part of NotedELN

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

// LineWidthBar.C

#include "LineWidthBar.h"
#include "LineWidthItem.h"
#include "Mode.h"
#include "math.h"

static double lineWidths[] = {
  0.5,
  1,
  1.5,
  2,
  3,
  5,
  8,
};
static int nLineWidths = sizeof(lineWidths)/sizeof(*lineWidths);

LineWidthBar::LineWidthBar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<nLineWidths; i++) 
    addTool(widthToId(lineWidths[i]), new LineWidthItem(lineWidths[i]));

  QString id = widthToId(mode->lineWidth());
  mode->setLineWidth(idToWidth(id));
  select(id);
  setColor(mode->color());
}

LineWidthBar::~LineWidthBar() {
}

void LineWidthBar::setColor(QColor c) {
  foreach (QGraphicsItem *gi, childItems()) {
    LineWidthItem *i = dynamic_cast<LineWidthItem *>(gi);
    if (i)
      i->setColor(c);
  }
}

void LineWidthBar::doLeftClick(QString id, Qt::KeyboardModifiers) {
  mode->setLineWidth(idToWidth(id));
}

double LineWidthBar::idToWidth(QString s) {
  return s.toDouble();
}

QString LineWidthBar::widthToId(double w) {
  double bw = 0.0001;
  for (int k=0; k<nLineWidths; k++) 
    if (fabs(w-lineWidths[k])/lineWidths[k] <
	fabs(w-bw)/bw)
      bw = lineWidths[k];
  return QString::number(bw, 'f', 2);
}


