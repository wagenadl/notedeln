// GfxImageItem.H

#ifndef GFXIMAGEITEM_H

#define GFXIMAGEITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include "Item.H"

class GfxImageItem: public QObject, public QGraphicsPixmapItem, public Item {
  Q_OBJECT;
public:
  GfxImageItem(class GfxImageData *data, Item *parent=0);
  virtual ~GfxImageItem();
  QRectF boundingRect() const;
  QRectF imageBoundingRect() const; // w/o margins
  virtual void makeWritable();
  virtual void setScale(double);
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
signals:
  void newScale(double);
private slots:
  void modifierChange(Qt::KeyboardModifiers);
private:
  QPointF moveDelta(QGraphicsSceneMouseEvent *);
  DragType dragTypeForPoint(QPointF);
  Qt::CursorShape cursorForDragType(DragType);
  void setCursor(Qt::CursorShape);
private:
  GfxImageData *data;
  QImage image; // not cropped
  DragType dragType;
  QPointF dragStart; // in parent block's coordinates
  Qt::CursorShape oldCursor;
  QPointF cursorPos;
  QRectF dragCrop; // in image coords
};

#endif
