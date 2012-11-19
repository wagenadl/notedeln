// HoverRegion.H

#ifndef HOVERREGION_H

#define HOVERREGION_H

#include <QGraphicsItem>

class HoverRegion: public QGraphicsItem {
public:
  HoverRegion(class MarkupData *md, class TextItem *item);
  virtual ~HoverRegion();
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  virtual QPainterPath shape() const;
  void updateShape(bool force=false);
protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private:
  QString refText() const;
private:
  MarkupData *md;
  TextItem *ti;
  QPainterPath bounds;
  class PreviewPopper *popper;
  int start, end;
};

#endif
