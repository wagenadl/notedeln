// ClockFace.cpp

#include "ClockFace.h"

#include <QTime>
#include <QPainter>
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <math.h>

ClockFace::ClockFace(QGraphicsItem *parent): QGraphicsObject(parent) {
  scale = 100;
  foregroundColor = QColor("#dddddd");
  shadowColor = QColor("#888888");
  auto ef = new QGraphicsDropShadowEffect;
  ef->setXOffset(.5);
  ef->setYOffset(.5);
  ef->setBlurRadius(5);
  ef->setColor(shadowColor);
  setGraphicsEffect(ef);
  startTimer(10000);
}

void ClockFace::setScale(double r) {
  prepareGeometryChange();
  scale = r;
  update();
}

void ClockFace::setForegroundColor(QColor fg) {
  foregroundColor = fg;
  update();
}

void ClockFace::setShadowColor(QColor sh) {
  shadowColor = sh;
  update();
}

QRectF ClockFace::boundingRect() const {
  return QRectF(QPointF(0, 0), QSizeF(scale, scale));
}

void ClockFace::paint(QPainter *p,
		      const QStyleOptionGraphicsItem *, QWidget *) {
  QTime time = QTime::currentTime();
  double h = time.hour() + time.minute() / 60.;
  double m = time.minute() + time.second() / 60.;
  
  QPointF center(scale/2, scale/2);
  double radius = scale/2;

  QPen pen;
  pen.setColor(foregroundColor);
  pen.setCapStyle(Qt::RoundCap);
  
  // pen.setWidthF(scale/20);
  // p->setPen(pen);
  // p->drawEllipse(center, .45*scale, .45*scale);

  pen.setWidthF(scale/30);
  p->setPen(pen);
  for (int k=0; k<12; k++) {
    double phi = k * 3.141592 * 2 / 12;
    QPointF xy = center + QPointF(.45*cos(phi)*radius, .45*sin(phi)*radius);
    p->drawPoint(xy);
  }

  // hour hand
  pen.setWidthF(scale/25);
  pen.setColor(foregroundColor.lighter(101));
  p->setPen(pen);
  double phi = h * 3.141592 * 2 / 12;
  QPointF dxy = QPointF(.35*sin(phi)*radius, -.35*cos(phi)*radius);
  p->drawLine(center, center + dxy);

  // minute hand
  pen.setWidthF(scale/40);
  pen.setColor(foregroundColor.lighter(102));
  p->setPen(pen);
  phi = m * 3.141592 * 2 / 60;
   dxy = QPointF(.45*sin(phi)*radius, -.45*cos(phi)*radius);
  p->drawLine(center, center + dxy);
}

void ClockFace::timerEvent(QTimerEvent *) {
  update();
}
