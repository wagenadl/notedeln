// GfxImageItem.H

#ifndef GFXIMAGEITEM_H

#define GFXIMAGEITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsObject>
#include "Item.H"
#include "Mode.H"
#include "GfxImageData.H"

class GfxImageItem: public Item {
  Q_OBJECT;
public:
  GfxImageItem(GfxImageData *data, Item *parent=0);
  virtual ~GfxImageItem();
  DATAACCESS(GfxImageData);
  QRectF boundingRect() const;
  QRectF imageBoundingRect() const; // w/o margins
  virtual void makeWritable();
  virtual void setScale(double);
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
private:
  enum DragType {
    None,
    Move,
    ResizeTopLeft,
    ResizeTopRight,
    ResizeBottomLeft,
    ResizeBottomRight,
    CropLeft,
    CropRight,
    CropTop,
    CropBottom,
  };
private slots:
  void modeChange(Mode::M);
private:
  QPointF moveDelta(QGraphicsSceneMouseEvent *);
  DragType dragTypeForPoint(QPointF);
  Qt::CursorShape cursorForDragType(DragType);
  void setCursor(Qt::CursorShape);
private:
  QImage image; // not cropped
  DragType dragType;
  QPointF dragStart; // in parent block's coordinates
  Qt::CursorShape oldCursor;
  QPointF cursorPos;
  QRectF cropStart; // in parent block's coordinates
  QRectF imStart; // in parent block's coordinates
  QRect dragCrop;
  QGraphicsPixmapItem *pixmap;
};

#endif
