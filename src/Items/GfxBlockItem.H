// GfxBlockItem.H

#ifndef GFXBLOCKITEM_H

#define GFXBLOCKITEM_H

#include "BlockItem.H"
#include <Qt>
#include <QUrl>
#include "GfxBlockData.H"

class GfxBlockItem: public BlockItem {
  Q_OBJECT;
public:
  GfxBlockItem(GfxBlockData *data, Item *parent=0);
  virtual ~GfxBlockItem();
  DATAACCESS(GfxBlockData);
  Item *newImage(QImage img, QUrl src, QPointF xy);
  Item *newImage(QUrl src, QPointF xy);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF netChildBoundingRect() const;
  /* Our "netChildBoundingRect" also includes a basic 6"x1" rectangle; a
     place holder so that empty GfxBlockItems retain finite size.
   */
  virtual void makeWritable();
public: // should be protected, but PageScene calls it
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  double availableWidth() const;
  void drawGrid(QPainter *p, QRectF const &bb, double dx);
  void createMark(QPointF pos);
};

#endif
