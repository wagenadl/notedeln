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
  QList<QGraphicsItem *> const &gfx();
  bool isEmpty() const;
  class GfxImageItem *newImage(QImage img, QUrl const *src, QPointF xy);
  class GfxImageItem *newImage(QUrl src, QPointF xy);
  virtual QRectF netBoundingRect() const; // w/o notes
  void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
  static Qt::KeyboardModifiers moveModifiers(); // mask of modifier keys ...
  static Qt::MouseButton moveButton(); // ... and mouse button that should
                                       // trigger a move
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private:
  double availableWidth() const;
  void growToFit();
  void drawGrid(QPainter *p, QRectF const &bb, double dx);
private:
  GfxBlockData *data_; // we don't own
  QList<QGraphicsItem *> items_; // we do own
};

#endif
