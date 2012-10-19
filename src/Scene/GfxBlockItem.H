// GfxBlockItem.H

#ifndef GFXBLOCKITEM_H

#define GFXBLOCKITEM_H

#include "BlockItem.H"
#include <Qt>

class GfxBlockItem: public BlockItem {
  Q_OBJECT;
public:
  GfxBlockItem(class GfxBlockData *data, class PageScene *parent);
  virtual ~GfxBlockItem();
  GfxBlockData *data(); // benevolently hides inherited version
  class GfxImageItem *newImage(QImage img, QUrl const *src, QPointF xy);
  class GfxImageItem *newImage(QUrl src, QPointF xy);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
  virtual void childGeometryChanged();
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private:
  void sizeToFit();
  double availableWidth() const;
  void drawGrid(QPainter *p, QRectF const &bb, double dx);
private:
  GfxBlockData *data_; // we don't own
  class PreliminaryLine *dragLine; // null unless dragging
};

#endif
