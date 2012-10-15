// GfxBlockItem.H

#ifndef GFXBLOCKITEM_H

#define GFXBLOCKITEM_H

#include "BlockItem.H"

class GfxBlockItem: public BlockItem {
  Q_OBJECT;
public:
  GfxBlockItem(class GfxBlockData *data, class PageScene *parent);
  virtual ~GfxBlockItem();
  GfxBlockData *data(); // benevolently hides inherited version
  QList<QGraphicsItem *> const &gfx();
  bool isEmpty() const;
  class GfxImageItem *newImage(QString resName, QPointF xy);
  virtual QRectF netBoundingRect() const; // w/o notes
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
private:
  double width() const;
  double height() const;
private:
  GfxBlockData *data_; // we don't own
  QList<QGraphicsItem *> items_; // we do own
};

#endif
