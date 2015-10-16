// ClockFace.h

#ifndef CLOCKFACE_H

#define CLOCKFACE_H

#include <QGraphicsObject>

class ClockFace: public QGraphicsObject {
  Q_OBJECT;
public:
  ClockFace(QGraphicsItem *parent=0);
  virtual ~ClockFace() {}
  void setScale(double r);
  void setForegroundColor(QColor fg);
  void setShadowColor(QColor sh);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void timerEvent(QTimerEvent *);
private:
  double scale;
  QColor foregroundColor;
  QColor shadowColor;
};

#endif
