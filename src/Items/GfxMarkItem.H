// GfxMarkItem.H

#ifndef GFXMARKITEM_H

#define GFXMARKITEM_H

#include "Item.H"
#include "GfxMarkData.H"
#include "Mode.H"

#include <QGraphicsObject>

class GfxMarkItem: public Item {
  Q_OBJECT;
public:
  GfxMarkItem(GfxMarkData *data, Item *parent=0);
  virtual ~GfxMarkItem();
  DATAACCESS(GfxMarkData);
  QRectF boundingRect() const;
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void makeWritable();
  static GfxMarkItem *newMark(QPointF p,
			      QColor c, double siz, GfxMarkData::Shape shp,
			      Item *parent);
  static GfxMarkItem *newMark(QPointF p, Item *parent);
  static void renderMark(QPointF p0,
			 QColor c, double siz, GfxMarkData::Shape shp,
			 QPainter *p);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private slots:
  void modeChange(Mode::M);
};

#endif
