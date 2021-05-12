// Scenes/RoundedRect.cpp - This file is part of NotedELN

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

// RoundedRect.C

#include "RoundedRect.h"
#include <QPainter>

RoundedRect::RoundedRect(QGraphicsItem *parent):
  QGraphicsObject(parent) {
  width_ = 200;
  height_ = 100;
  radius_ = 0;
  dx_ = dy_ = 1.5;
  blackalpha_ = 0.2;
  whitealpha_ = 1;
  
}

RoundedRect::~RoundedRect() {
}

void RoundedRect::setBlackAlpha(double a) {
  blackalpha_ = a;
  update();
}

void RoundedRect::setWhiteAlpha(double a) {
  whitealpha_ = a;
  update();
}
void RoundedRect::resize(QSizeF s) {
  resize(s.width(), s.height());
}

void RoundedRect::resize(double w, double h) {
  prepareGeometryChange();
  width_ = w;
  height_ = h;
  update();
}
  
void RoundedRect::setRadius(double r) {
  radius_ = r;
  update();
}

void RoundedRect::setOffset(double dx) {
  setOffset(dx, dx);
}

void RoundedRect::setOffset(double dx, double dy) {
  dx_ = dx;
  dy_ = dy;
  update();
}

QRectF RoundedRect::boundingRect() const {
  return QRectF(0, 0, width_, height_);
}

void RoundedRect::paint(QPainter *p,
			const QStyleOptionGraphicsItem *, QWidget *) {
  QColor blk("black");
  blk.setAlphaF(blackalpha_);
  QColor wht("white");
  wht.setAlphaF(whitealpha_);
  p->setPen(Qt::NoPen);

  p->setBrush(blk);
  p->drawRoundedRect(QRectF(0, 0, width_, height_),
		     radius_, radius_);

  p->setBrush(wht);
  p->drawRoundedRect(QRectF(dx_, dy_, width_-dx_, height_-dy_),
		     radius_, radius_);
}
