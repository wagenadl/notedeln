// VideoSlider.cpp

#include "VideoSlider.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QGraphicsDropShadowEffect>

VideoSlider::VideoSlider():
  dur_s(60), pos_s(0), width(100), height(10) {
  line = new QGraphicsLineItem(0, height/2, width, height/2);
  line->setParentItem(this);
  line->setPen(QPen(QColor(255,255,128), height/5));
  line->setAcceptedMouseButtons(0);
  ellipse = new QGraphicsEllipseItem(0, 0, height, height);
  ellipse->setParentItem(this);
  ellipse->setPen(QPen(Qt::NoPen));
  ellipse->setBrush(QBrush(QColor(255,255,128)));
  ellipse->setAcceptedMouseButtons(0);
  QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect;
  eff->setColor(QColor(0,0,0));
  eff->setOffset(QPointF(0, 0));
  eff->setBlurRadius(6);
  setGraphicsEffect(eff);
}

VideoSlider::~VideoSlider() {
}

double VideoSlider::time2x(double t_s) const {
  return height/2 + (width - height) * t_s / dur_s;
}

double VideoSlider::x2time(double x) const {
  double t_s = (x - height/2) / (width - height) * dur_s;
  return t_s<0 ? 0
    : t_s>dur_s ? dur_s
    : t_s;
}

void VideoSlider::moveEllipse() {
  ellipse->setRect(time2x(pos_s) - height/2, 0, height, height);
}

void VideoSlider::setDuration(double t_s) {
  dur_s = t_s;
  moveEllipse();
  update();
}

void VideoSlider::setPosition(double t_s) {
  pos_s = t_s;
  moveEllipse();
  update();
}

void VideoSlider::resize(double w, double h) {
  prepareGeometryChange();
  width = w;
  height = h;
  line->setLine(0, height/2, width, height/2);
  line->setPen(QPen(QColor(255,255,128), height/5));
  moveEllipse();
  update();
}
  
void VideoSlider::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}

void VideoSlider::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "vs move" << e->pos();
  setPosition(x2time(e->pos().x()));
  emit sliderDragged(pos_s);
}

void VideoSlider::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "vs press" << e->pos();
  e->accept();
}

QRectF VideoSlider::boundingRect() const {
  return QRectF(QPointF(0,0), QSize(width, height));
}

