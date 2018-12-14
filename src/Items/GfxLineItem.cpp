// Items/GfxLineItem.cpp - This file is part of eln

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

// GfxLineItem.C

#include "GfxLineItem.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QEventLoop>
#include "Assert.h"
#include "BlockItem.h"
#include "Cursors.h"

#define MAX_DISTORT 3

static Item::Creator<GfxLineData, GfxLineItem> c("gfxline");

GfxLineItem::GfxLineItem(GfxLineData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
  rebuildPath();
  building = false;
}

GfxLineItem::~GfxLineItem() {
}


QPointF GfxLineItem::roundToGrid(QPointF p) const {
  double dx = style().real("canvas-grid-spacing");
  return QPointF(dx*round(p.x()/dx), dx*round(p.y()/dx));
}

void GfxLineItem::rebuildPath() {
  QList<double> const &xx = data()->xx();
  QList<double> const &yy = data()->yy();
  ASSERT(xx.size() == yy.size());
  if (xx.isEmpty()) {
    path = QPainterPath();
  } else {
    path = QPainterPath(QPointF(xx[0], yy[0]));
    for (int i=1; i<xx.size(); i++)
      path.lineTo(xx[i], yy[i]);
  }
  QPainterPathStroker stroker;
  stroker.setWidth(data()->lineWidth() + 6);
  stroked = stroker.createStroke(path);
}
  
QRectF GfxLineItem::boundingRect() const {
  return path.boundingRect().adjusted(-2, -2, 2, 2);
}

QPainterPath GfxLineItem::shape() const {
  return stroked;
}

void GfxLineItem::paint(QPainter *p,
			  const QStyleOptionGraphicsItem *,
			  QWidget *) {
  QPen pen(data()->color());
  pen.setWidthF(data()->lineWidth());
  p->setPen(pen);
  p->drawPath(path);
}

GfxLineItem *GfxLineItem::newLine(QPointF p, Item *parent) {
  return newLine(p,
		 parent->mode()->color(),
		 parent->mode()->lineWidth(),
		 parent);
}

GfxLineItem *GfxLineItem::newLine(QPointF p,
					QColor c, double lw,
					Item *parent) {
  GfxLineData *gmd = new GfxLineData(parent->data());
  
  gmd->setPos(p);
  gmd->setColor(c);
  gmd->setLineWidth(lw);
  GfxLineItem *gmi = new GfxLineItem(gmd, parent);
  gmi->makeWritable();
  return gmi;
}

void GfxLineItem::build(QGraphicsSceneMouseEvent *e) {
  // e->pos is in terms of parents coords!
  building = true;
  bool usegrid = !(e->modifiers() & Qt::ControlModifier);
  data()->clear();
  if (usegrid) {
    data()->setPos(roundToGrid(data()->pos()));
    setPos(data()->pos());
  }
  data()->addPoint(QPointF(), true); // by def., we start at the origin
  data()->addPoint(QPointF(), true); // and preliminarily end at the origin
  QEventLoop el;
  connect(this, SIGNAL(doneBuilding()),
	  &el, SLOT(quit()),
	  Qt::QueuedConnection);
  grabMouse();
  el.exec();
  ungrabMouse();
  building = false;
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  data()->markModified();
}
  
void GfxLineItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    moveBuilding(e);
    emit doneBuilding();
  } else {
    if (isWritable()
	&& (mode()->mode()==Mode::MoveResize
	    || (e->modifiers() & Qt::ControlModifier))) {
      e->accept();
    } else {
      QGraphicsObject::mousePressEvent(e);
    }
  }
}

static double distance(QLineF l, QPointF p) {
  // distance between point and line segment
  /* Parametrize the line L as:
     x(t) = x0 + t*(x1-x0)
     y(t) = y0 + t*(y1-y0)
     Minimize:
     r(t) = (p_x - x(t))^2 + (p_y - y(t))^2
     That is:
     dr/dt = 0 = 2*(x(t)-p_x)*(x1-x0) + 2*(y(t)-p_y*(y1-y0)
     That is:
     (x0+t*(x1-x0)-p_x)*(x1-x0) + (y0+t*(y1-y0)-p_y)*(y1-y0) = 0
     Or:
     t = [(x0-p_x)*(x1-x0) + (y0-p_y)*(y1-y0)] /
           [(x1-x0)^2 + (y1-y0)^2].
     Of course, if t<0, take t=0 and if t>1 take t=1.
  */
  double x0 = l.p1().x();
  double x1 = l.p2().x();
  double y0 = l.p1().y();
  double y1 = l.p2().y();
  double dx = x1-x0;
  double dy = y1-y0;
  double t = ((x0-p.x())*dx + ((y0-p.y())*dy)) / (dx*dx + dy*dy);
  if (t<0)
    t=0;
  else if (t>1)
    t=1;
  dx = x0 + t*dx - p.x();
  dy = y0 + t*dy - p.y();
  return sqrt(dx*dx + dy*dy);
}

void GfxLineItem::moveBuilding(QGraphicsSceneMouseEvent *e) {
  QPointF p = e->pos();
  bool usegrid = !(e->modifiers() & Qt::ControlModifier);
  if (usegrid)
    p = roundToGrid(p);
  
  data()->setPoint(1, p, true);
  prepareGeometryChange();
  rebuildPath();
  update();
}

void GfxLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    moveBuilding(e);
  } else {
    setPos(mapToParent(e->scenePos() - e->lastScenePos()));
    e->accept();
  }
}

void GfxLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    ;
  } else {
    data()->setPos(pos());
    if (ancestralBlock())
      ancestralBlock()->sizeToFit();
    e->accept();
  }
}

Qt::CursorShape GfxLineItem::cursorShape(Qt::KeyboardModifiers m) const {
  if (mode()->mode()==Mode::MoveResize
      || (m & Qt::ControlModifier))
    return Qt::SizeAllCursor;
  else 
    return Qt::CrossCursor;
}

bool GfxLineItem::changesCursorShape() const {
  return true;
}

void GfxLineItem::makeWritable() {
  Item::makeWritable();
}
