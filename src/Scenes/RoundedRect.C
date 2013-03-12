// RoundedRect.C

#include "RoundedRect.H"
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
