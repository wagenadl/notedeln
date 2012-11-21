// HoverRegion.H

#ifndef HOVERREGION_H

#define HOVERREGION_H

#include <QGraphicsObject>

class HoverRegion: public QGraphicsObject {
  Q_OBJECT;
public:
  HoverRegion(class MarkupData *md, class TextItem *item,
	      QGraphicsItem *parent);
  virtual ~HoverRegion();
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  virtual QPainterPath shape() const;
  void forgetBounds();
protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private:
  QString refText() const;
  void calcBounds() const;
private:
  MarkupData *md;
  TextItem *ti;
  mutable QPainterPath bounds;
  class PreviewPopper *popper;
  mutable int start;
  mutable int end;
};

#endif
