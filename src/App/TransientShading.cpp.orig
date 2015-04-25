// TransientShading.cpp - This file is part of eln

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

#include "TransientShading.h"
#include "TextItem.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextLine>
#include <QDebug>

TransientShading::TransientShading(TextItem *item, int start, int end,
				   QGraphicsItem *parent):
  QGraphicsObject(parent), ti(item), start(start), end(end) {
  calcBounds();
}

TransientShading::~TransientShading() {
}

QRectF TransientShading::boundingRect() const {
  return bounds.boundingRect();
}

void TransientShading::paint(QPainter *p,
			    const QStyleOptionGraphicsItem *,
			    QWidget *) {
  p->setPen(Qt::NoPen);
  QColor c = ti->style().color("transientshading-color");
  c.setAlpha(int(255*ti->style().real("transientshading-alpha")));
  p->setBrush(c);
  p->drawPath(bounds); 
}

QPainterPath TransientShading::shape() const {
  return bounds;
}

void TransientShading::calcBounds() {
  bounds = QPainterPath();
  int pos = start;
  while (pos<end) {
    QTextBlock tb(ti->document()->findBlock(pos));
    ASSERT(tb.isValid());
    int rpos = pos-tb.position();
    QTextLayout *tlay = tb.layout();
    QTextLine line = tlay->lineForTextPosition(rpos);
    if (!line.isValid()) {
      qDebug() << "TransientShading: Invalid line";
      break;
    }
    double y0 = tlay->position().y() + line.y();
    double y1 = y0 + line.height();
    double x0 = tlay->position().x() + line.cursorToX(rpos);
    int lineEnd = line.textStart()+line.textLength();
    int rend = end-tb.position();
    double x1 = tlay->position().x() + (rend<lineEnd
					? line.cursorToX(rend)
					: line.cursorToX(lineEnd));
    QRectF bit(QPointF(x0, y0), QPointF(x1, y1));
    bit = mapRectFromScene(ti->mapRectToScene(bit));
    bounds.addRect(bit);
    pos = tb.position() + lineEnd;
  }
}    
