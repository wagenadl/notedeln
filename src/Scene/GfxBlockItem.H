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
  Item *newImage(QImage img, QUrl const *src, QPointF xy);
  Item *newImage(QUrl src, QPointF xy);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
  virtual void childGeometryChanged();
  virtual void makeWritable();
protected slots:
  virtual void modifierChange(Qt::KeyboardModifiers);
private:
  void sizeToFit();
  double availableWidth() const;
  void drawGrid(QPainter *p, QRectF const &bb, double dx);
private:
  GfxBlockData *data_;
};

#endif
