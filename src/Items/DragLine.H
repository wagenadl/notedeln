// DragLine.H

#ifndef DRAGLINE_H

#define DRAGLINE_H

#include <QGraphicsLineItem>
#include <QGraphicsItem>

class DragLine: public QObject, public QGraphicsLineItem {
  Q_OBJECT;
public:
  DragLine(QPointF p0, QGraphicsItem *parent);
  virtual ~DragLine();
  QPointF startPoint() const;
  QPointF endPoint() const;
  void setEndPoint(QPointF p1);
  bool isShort() const; // true if trivially short
signals:
  void release();
public:
  static QPointF drag(class Item *parent, QPointF p0, QColor c=QColor());
  /* drag() returns the end point of the dragged line, except if the line
     is short (according to isShort), in which case the start point is
     returned.
  */
  static bool isShort(QPointF p0, QPointF p1);
  static double shortThreshold();
protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
};

#endif
