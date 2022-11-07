// App/PreviewPopper.cpp - This file is part of NotedELN

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

// PreviewPopper.C

#include "PreviewPopper.h"
#include "Resource.h"
#include <QApplication>
#include <QScreen>

#include <QLabel>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>
#include "ElnAssert.h"
#include "PopLabel.h"
#include <QTimer>

PreviewPopper::PreviewPopper(Resource *res,
			     QRect over, QObject *parent):
  QObject(parent), res(res), over(over) {
  widget = 0;
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), SLOT(timeout()));
  timer->setSingleShot(true);
  timer->start(100);
}

PreviewPopper::~PreviewPopper() {
  if (widget)
    delete widget;
}

void PreviewPopper::timeout() {
  popup();
}

QWidget *PreviewPopper::popup() {
  if (widget) {
    smartPosition();
    widget->show();
    return widget;
  }

  QPixmap p;
  if (res->hasPreview())
    p.load(res->previewPath());

  if (!p.isNull()) {
    widget = new PopLabel;
    widget->setPixmap(p);
  } else if (!res->title().isEmpty()) {
    widget = new PopLabel;
    widget->setText(res->title());
  }
  if (widget) {
    connect(widget, SIGNAL(clicked(Qt::KeyboardModifiers)),
	    this, SIGNAL(clicked(Qt::KeyboardModifiers)));
    connect(widget, SIGNAL(left()),
	    this, SLOT(closeAndDie()));
    widget->resize(widget->sizeHint());
    smartPosition();
    widget->show();
    qDebug() << widget->pos();
  }
  return widget;
}

void PreviewPopper::smartPosition() {
  ASSERT(widget);
  
  QRect desktop = QApplication::primaryScreen()->availableGeometry();

  QSize s = widget->frameSize();

  /* We will attempt to position the popup so that it is away from the
     mouse pointer. There are several options:
     (1) below the mouse pointer and sticking out to the left and right
     (2) above the mouse pointer and sticking out to the left and right
     (3) to the right of the mouse pointer and stickout out up and down
     (4) to the left of  the mouse pointer and stickout out up and down
     We try those in order and maximize how much of the popup fits on the
     screen.
  */
  QPoint dest;
  QPoint bestDest;
  QRectF ir;
  double area;
  double bestArea = 0;
  int dy = 25;
  int dx = 50;

  QPoint center = QCursor::pos();
  qDebug() << "Cursor: " << center;

  // below
  dest = center + QPoint(-s.width()/3, dy);
  if (dest.x()+s.width()>desktop.right())
    dest.setX(desktop.right()-s.width());
  if (dest.x()<desktop.left())
    dest.setX(desktop.left());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  // above
  dest = center + QPoint(-s.width()/3, -dy-s.height());
  if (dest.x()+s.width()>desktop.right())
    dest.setX(desktop.right()-s.width());
  if (dest.x()<desktop.left())
    dest.setX(desktop.left());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  // to the right
  dest = center + QPoint(dx, -s.height()/3);
  if (dest.y()+s.height()>desktop.bottom())
    dest.setY(desktop.bottom()-s.height());
  if (dest.y()<desktop.top())
    dest.setY(desktop.top());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }

  // to the left
  dest = center + QPoint(-dx-s.width(), -s.height()/3);
  if (dest.y()+s.height()>desktop.bottom())
    dest.setY(desktop.bottom()-s.height());
  if (dest.y()<desktop.top())
    dest.setY(desktop.top());
  ir = QRectF(dest, s).intersected(desktop);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestDest = dest;
    bestArea = area;
  }
  qDebug() << "Bestdest" << bestDest << " area" << bestArea << s;

  widget->move(bestDest);
}

void PreviewPopper::closeSoon() {
  if (widget)
    widget->closeSoon();
}

void PreviewPopper::closeAndDie() {
  qDebug() << "Close and die";
  deleteLater();
}
