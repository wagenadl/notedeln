// Items/GfxLineItem.cpp - This file is part of NotedELN

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

// GfxLineItem.C

#include "GfxLineItem.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QEventLoop>
#include "ElnAssert.h"
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
  connect(this, &GfxLineItem::doneBuilding,
	  &el, &QEventLoop::quit,
	  Qt::QueuedConnection);
  grabMouse();
  el.exec();
  ungrabMouse();
  building = false;
  if (ancestralBlock())
    ancestralBlock()->sizeToFit();
  data()->markModified();
}

static double L2(QPointF const &a, QPointF const &b) {
  double dx = a.x() - b.x();
  double dy = a.y() - b.y();
  return dx*dx + dy*dy;
}
  
void GfxLineItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    ;
  } else {
    if (isWritable()
	&& (mode()->mode()==Mode::MoveResize
	    || (e->modifiers() & Qt::ControlModifier))) {
      e->accept();
      double d0 = L2(e->pos(), data()->point(0));
      double d1 = L2(e->pos(), data()->point(1));
      double len = L2(data()->point(1), data()->point(0));
      if (d0<d1 && d0<.1*len) {
        pressidx = 0;
        presspt = data()->point(0);
      } else if (d1<d0 && d1<.1*len) {
        pressidx = 1;
        presspt = data()->point(1);
      } else {
        pressidx = -1;
        presspt = data()->point(0);
        presspt2 = data()->point(1);
      }
    } else {
      QGraphicsObject::mousePressEvent(e);
    }
  }
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
  } else if (e->buttons() & Qt::LeftButton) {
    QPointF delta = e->pos() - e->buttonDownPos(Qt::LeftButton);
    bool usegrid = !(e->modifiers() & Qt::ControlModifier);
    QPointF p = presspt + delta;
    if (usegrid)
      p = roundToGrid(p);
    if (pressidx>=0) {
      data()->setPoint(pressidx, p);
    } else {
      data()->setPoint(0, p);
      p = presspt2 + delta;
      if (usegrid)
        p = roundToGrid(p);
      data()->setPoint(1, p);
    }
    prepareGeometryChange();
    rebuildPath();
      //      setPos(mapToParent(e->scenePos() - e->lastScenePos()));
    e->accept();
  }
}

void GfxLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    moveBuilding(e);
    emit doneBuilding();
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
