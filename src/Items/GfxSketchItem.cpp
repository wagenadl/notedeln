// Items/GfxSketchItem.cpp - This file is part of eln

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

// GfxSketchItem.C

#include "GfxSketchItem.h"
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

static Item::Creator<GfxSketchData, GfxSketchItem> c("gfxsketch");

GfxSketchItem::GfxSketchItem(GfxSketchData *data, Item *parent):
  Item(data, parent) {
  setPos(data->pos());
  rebuildPath();
  building = false;
}

GfxSketchItem::~GfxSketchItem() {
}

void GfxSketchItem::rebuildPath() {
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
  stroker.setWidth(data()->lineWidth() + 1);
  stroked = stroker.createStroke(path);
}
  
QRectF GfxSketchItem::boundingRect() const {
  return path.boundingRect();
}

QPainterPath GfxSketchItem::shape() const {
  return stroked;
}

void GfxSketchItem::paint(QPainter *p,
			  const QStyleOptionGraphicsItem *,
			  QWidget *) {
  QPen pen(data()->color());
  pen.setWidthF(data()->lineWidth());
  p->setPen(pen);
  p->drawPath(path);
}

GfxSketchItem *GfxSketchItem::newSketch(QPointF p, Item *parent) {
  return newSketch(p,
		   parent->mode()->color(),
		   parent->mode()->lineWidth(),
		   parent);
}

GfxSketchItem *GfxSketchItem::newSketch(QPointF p,
					QColor c, double lw,
					Item *parent) {
  GfxSketchData *gmd = new GfxSketchData(parent->data());
  gmd->setPos(p);
  gmd->setColor(c);
  gmd->setLineWidth(lw);
  GfxSketchItem *gmi = new GfxSketchItem(gmd, parent);
  gmi->makeWritable();
  return gmi;
}

void GfxSketchItem::build() {
  building = true;
  data()->clear();
  data()->addPoint(QPointF(0, 0), true); // by def., we start at the origin
  droppedPoints.clear();
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
  
void GfxSketchItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (building)
    return;
  if (isWritable() && mode()->mode()==Mode::MoveResize) {
    e->accept();
  } else {
    QGraphicsObject::mousePressEvent(e);
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
  

void GfxSketchItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    QPointF p = e->pos();
    QList<double> const &xx = data()->xx();
    QList<double> const &yy = data()->yy();
    int N = xx.size();
    bool okToDropPrevious = false;
    if (N>=2) {
      /* We already have at least two points, so we could consider dropping
	 the previous point if that doesn't cause trouble. Here, "trouble"
         means more than some acceptable distortion. */
      QLineF l(xx[N-2], yy[N-2], p.x(), p.y());
      droppedPoints.append(QPointF(xx[N-1], yy[N-1]));
      okToDropPrevious = true;
      foreach (QPointF p, droppedPoints) {
	if (distance(l, p) > MAX_DISTORT) {
	  okToDropPrevious = false;
	  break;
	}
      }
    }
    if (okToDropPrevious) {
      data()->setPoint(N-1, p, true);
    } else {
      droppedPoints.clear();
      data()->addPoint(p, true);
    }
    prepareGeometryChange();
    rebuildPath();
    update();
  } else {
    setPos(mapToParent(e->scenePos() - e->lastScenePos()));
    e->accept();
  }
}

void GfxSketchItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  if (building) {
    emit doneBuilding();
  } else {
    data()->setPos(pos());
    if (ancestralBlock())
      ancestralBlock()->sizeToFit();
    e->accept();
  }
}


Qt::CursorShape GfxSketchItem::cursorShape() const {
  if (mode()->mode()==Mode::MoveResize)
    return Qt::SizeAllCursor;
  else 
    return Qt::CrossCursor;
}

bool GfxSketchItem::changesCursorShape() const {
  return true;
}

void GfxSketchItem::makeWritable() {
  Item::makeWritable();
}
