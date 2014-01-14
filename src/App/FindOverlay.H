// FindOverlay.H

#ifndef FINDOVERLAY_H

#define FINDOVERLAY_H

#include <QGraphicsObject>

class FindOverlay: public QGraphicsObject {
  Q_OBJECT;
public:
  FindOverlay(QGraphicsScene *scene, QString phrase);
  virtual ~FindOverlay();
  virtual QRectF boundingRect() const { return QRectF(); }
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget*) {}
  
protected:
  void timerEvent(QTimerEvent *); // self-destruct
};

#endif
