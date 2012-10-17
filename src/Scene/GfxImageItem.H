// GfxImageItem.H

#ifndef GFXIMAGEITEM_H

#define GFXIMAGEITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

class GfxImageItem: public QObject, public QGraphicsPixmapItem {
  Q_OBJECT;
public:
  GfxImageItem(class GfxImageData *data, QGraphicsObject *parent=0);
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
private slots:
  void modifierChange(Qt::KeyboardModifiers);
private:
  class GfxBlockItem *parentBlock() const;
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
  bool moveModPressed;
};

#endif
