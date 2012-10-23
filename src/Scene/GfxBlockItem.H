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
  Item *newNote(QPointF p0);
  Item *newNote(QPointF p0, QPointF p1);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
  virtual void childGeometryChanged();
  virtual void makeWritable();
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
protected slots:
  virtual void modifierChange(Qt::KeyboardModifiers);
private slots:
  void noteAbandoned();
private:
  void sizeToFit();
  double availableWidth() const;
  void drawGrid(QPainter *p, QRectF const &bb, double dx);
private:
  GfxBlockData *data_; // we don't own
  class PreliminaryLine *dragLine; // null unless dragging
};

#endif
