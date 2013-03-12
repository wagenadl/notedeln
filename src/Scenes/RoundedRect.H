// RoundedRect.H

#ifndef ROUNDEDRECT_H

#define ROUNDEDRECT_H

#include <QGraphicsObject>

class RoundedRect: public QGraphicsObject {
  Q_OBJECT;
public:
  RoundedRect(QGraphicsItem *parent=0);
  virtual ~RoundedRect();
  void setBlackAlpha(double);
  void setWhiteAlpha(double);
  void resize(QSizeF);
  void resize(double, double);
  void setRadius(double);
  void setOffset(double);
  void setOffset(double, double);
protected:
  QRectF boundingRect() const;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *);
private:
  double width_;
  double height_;
  double radius_;
  double dx_;
  double dy_;
  double blackalpha_;
  double whitealpha_;
};

#endif
