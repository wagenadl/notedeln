// Items/GfxMarkItem.cpp - This file is part of eln

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

// GfxMarkItem.C

#include "GfxMarkItem.h"
#include "BlockItem.h"
#include "Cursors.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

static Item::Creator<GfxMarkData, GfxMarkItem> c("gfxmark");

GfxMarkItem::GfxMarkItem(GfxMarkData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
}

GfxMarkItem::~GfxMarkItem() {
}

QRectF GfxMarkItem::boundingRect() const {
  double s = data()->size() + 1; // add +1 for line width
  return QRectF(-s,-s,2*s,2*s);
}

void GfxMarkItem::paint(QPainter *p,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
  renderMark(QPointF(0,0),
	     data()->color(), data()->size(), data()->shape(),
	     p);
}

void GfxMarkItem::renderMark(QPointF p0,
			     QColor c, double siz, GfxMarkData::Shape shp,
			     QPainter *p) {
  QPen pen;
  QBrush brush;
  QPointF delta(siz,siz);
  
  switch (shp) {
  case GfxMarkData::SolidCircle:
  case GfxMarkData::SolidSquare:
    pen = QPen(Qt::NoPen);
    brush = QBrush(c);
    break;
  default:
    pen = QPen(c);
    pen.setWidthF(sqrt(siz)/1.5);
    pen.setJoinStyle(Qt::MiterJoin);
    brush = QBrush(Qt::NoBrush);
    break;
  }

  p->setPen(pen);
  p->setBrush(brush);
  
  switch (shp) {
  case GfxMarkData::SolidCircle:
  case GfxMarkData::OpenCircle:
  case GfxMarkData::DotCircle:
    p->drawEllipse(QRectF(p0-delta, p0+delta));
    break;
  case GfxMarkData::SolidSquare:
  case GfxMarkData::OpenSquare:
  case GfxMarkData::DotSquare:
    p->drawRect(QRectF(p0-delta*.85, p0+delta*.85));
    break;
  case GfxMarkData::Cross:
    p->drawLine(p0 - QPointF(siz, siz)*.75, p0 + QPointF(siz, siz)*.75);
    p->drawLine(p0 - QPointF(siz, -siz)*.75, p0 + QPointF(siz, -siz)*.75);
    break;
  case GfxMarkData::Plus:
    p->drawLine(p0 - QPointF(siz, 0), p0 + QPointF(siz, 0));
    p->drawLine(p0 - QPointF(0, siz), p0 + QPointF(0, siz));
    break;
  }
  switch (shp) {
  case GfxMarkData::DotCircle:
    pen.setWidthF(sqrt(siz));
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->drawLine(p0-QPointF(1e-5,0), p0+QPointF(1e-5,0));
    break;
  case GfxMarkData::DotSquare:
    pen.setWidthF(sqrt(siz));
    pen.setCapStyle(Qt::SquareCap);
    p->setPen(pen);
    p->drawLine(p0-QPointF(1e-5,0), p0+QPointF(1e-5,0));
    break;
  default:
    break;
  }
}

GfxMarkItem *GfxMarkItem::newMark(QPointF p, Item *parent) {
  return newMark(p,
		 parent->mode()->color(),
		 parent->mode()->markSize(),
		 parent->mode()->shape(),
		 parent);
}

GfxMarkItem *GfxMarkItem::newMark(QPointF p,
				  QColor c, double siz, GfxMarkData::Shape shp,
				  Item *parent) {
  GfxMarkData *gmd = new GfxMarkData(parent->data());
  gmd->setPos(p);
  gmd->setColor(c);
  gmd->setSize(siz);
  gmd->setShape(shp);
  GfxMarkItem *gmi = new GfxMarkItem(gmd, parent);
  gmi->makeWritable();
  return gmi;
}
  
void GfxMarkItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (isWritable() && mode()->mode()==Mode::MoveResize) {
    e->accept();
  } else {
    QGraphicsObject::mousePressEvent(e);
  }
}

void GfxMarkItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  setPos(mapToParent(e->scenePos() - e->lastScenePos()));
  e->accept();
}

void GfxMarkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  data()->setPos(pos());
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  e->accept();
}


void GfxMarkItem::modeChange(Mode::M m) {
  if (m==Mode::MoveResize)
    setCursor(Qt::SizeAllCursor);
  else 
    setCursor(Cursors::crossCursor());
}

void GfxMarkItem::makeWritable() {
  connect(mode(), SIGNAL(modeChanged(Mode::M)),
	  SLOT(modeChange(Mode::M)));
  Item::makeWritable();
}
