// GfxImageItem.H

#ifndef GFXIMAGEITEM_H

#define GFXIMAGEITEM_H

#include <QGraphicsPixmapItem>

class GfxImageItem: public QGraphicsPixmapItem {
public:
  GfxImageItem(class GfxImageData *data, QGraphicsItem *parent=0);
  virtual ~GfxImageItem();
  void showCroppedAreas();
  void hideCroppedAreas();
  // whatever magic to crop and resize?
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
private:  
  QPointF moveDelta(QGraphicsSceneMouseEvent *);
  DragType dragTypeForPoint(QPointF);
  Qt::CursorShape cursorForDragType(DragType);
private:
  GfxImageData *data;
  QImage image; // not cropped
  DragType dragType;
  QPointF initialPos;
  QPointF dragStart;
  Qt::CursorShape oldCursor;
};

#endif
