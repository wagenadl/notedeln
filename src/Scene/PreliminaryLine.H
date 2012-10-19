// PreliminaryLine.H

#ifndef PRELIMINARYLINE_H

#define PRELIMINARYLINE_H

#include <QGraphicsLineItem>
#include <QGraphicsItem>

class PreliminaryLine: public QGraphicsItem {
public:
  PreliminaryLine(QPointF p0, QGraphicsItem *parent);
  virtual ~PreliminaryLine();
  void updateEnd(QPointF); // in parent coords
  QPointF startPoint() const;
  QPointF endPoint() const;
  bool isCircular() const; // not yet implemented: circular gesture
  bool isLine() const; // false if trivially short
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
private:
  QGraphicsLineItem *line;
  QPointF p0, p1;
};

#endif
