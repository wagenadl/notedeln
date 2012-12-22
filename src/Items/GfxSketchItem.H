// GfxSketchItem.H

#ifndef GFXSKETCHITEM_H

#define GFXSKETCHITEM_H

#include "Item.H"
#include "GfxSketchData.H"
#include "Mode.H"

#include <QGraphicsObject>

class GfxSketchItem: public Item {
  Q_OBJECT;
public:
  GfxSketchItem(GfxSketchData *data, Item *parent=0);
  virtual ~GfxSketchItem();
  GfxSketchData *data();
  virtual QRectF boundingRect() const;
  virtual QPainterPath shape() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual void makeWritable();
  static GfxSketchItem *newSketch(QPointF p,
				  QColor c, double lw,
				  Item *parent);
  static GfxSketchItem *newSketch(QPointF p, Item *parent);
  void build();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private slots:
  void modeChange(Mode::M);
private:
  void rebuildPath();
signals:
  void doneBuilding();
private:
  GfxSketchData *d;
  bool building;
  QList<QPointF> droppedPoints; // used during building
  QPainterPath path;
  QPainterPath stroked;
};

#endif
