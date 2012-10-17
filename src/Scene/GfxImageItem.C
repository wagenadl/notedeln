// GfxImageItem.C

#include "GfxImageItem.H"
#include "GfxItemFactory.H"
#include "GfxImageData.H"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include "ResourceManager.H"
#include <QCursor>

GfxItemFactory::Creator<GfxImageData, GfxImageItem> c("gfximage");

GfxImageItem::GfxImageItem(GfxImageData *data, QGraphicsItem *parent):
  QGraphicsPixmapItem(parent), data(data) {
  if (!data) {
    qDebug() << "GfxImageItem constructed w/o data";
    return;
  }

  dragType = None;

  // get the image, crop it, etc.
  ResourceManager *resmgr = data->resMgr();
  if (!resmgr) {
    qDebug() << "GfxImageItem: no resource manager";
    return;
  }
  if (!image.load(resmgr->path(data->resName()))) {
    qDebug() << "GfxImageItem: image load failed";
    return;
  }
  setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  setScale(data->scale());
  setPos(data->xy());
  setAcceptHoverEvents(true);
  oldCursor = Qt::ArrowCursor;
}

GfxImageItem::~GfxImageItem() {
}

void GfxImageItem::showCroppedAreas() {
  QPixmap pm(QPixmap::fromImage(image));
  setPixmap(pm);
  setOffset(-data->cropLeft(), -data->cropTop());
}

void GfxImageItem::hideCroppedAreas() {
  setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  setOffset(0, 0);
}

QPointF GfxImageItem::moveDelta(QGraphicsSceneMouseEvent *e) {
  QPointF delta = mapToParent(e->pos()) - dragStart;
  qDebug() << "delta: " << delta;
  return delta;
}

void GfxImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxImageItem::mouseMove" << e->pos();
  switch (dragType) {
  case None:
    qDebug() << " Nonmove!?";
    break;
  case Move:
    setPos(initialPos + moveDelta(e));
    break;
  default:
    qDebug() << "Crop NYI";
    break;
  }
}

void GfxImageItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxImageItem::mousePress" << e->pos() << e->modifiers();
  if (e->modifiers() & GfxBlockItem::moveModifiers()) {
    qDebug() << "-> I will move (in future, I might crop)";
    dragType = Move;
    initialPos = pos();
    /* Eventually, we'll use initialPos for multiple things:
       (1) for Move: initial TL in parent coords
       (2) for Resize: initial whatever corner in parent coords
       (3) for Crop: initial whatever edge in our coords (!)
     * Resize will be initiated if we are within 1/4 of width and 1/4 of
       height from a corner.
     * Crop will be initiated if we are not within resize distance of a
       corner and we are within 1/6 of width from a vertical edge or within
       1/6 of height from a horizontal edge.
     * It would be nice to have a graphical way to show what will happen.
       Can I make the mouse cursor reflect that when the relevant modifier is
       held down? YES! Hover events would allow that
    */

    dragStart = mapToParent(e->pos());
    e->accept();
  } else {
    e->ignore(); // pass to parent
  }
}

void GfxImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxImageItem::mouseRelease" << e->pos();
  switch (dragType) {
  case None:
    qDebug() << "Nonmove!?";
    break;
  case Move: {
    setPos(initialPos + moveDelta(e));
    data->setXY(pos());
    GfxBlockItem *block = dynamic_cast<GfxBlockItem *>(parentItem());
    if (block)
      block->checkVbox();
    else
      qDebug() << "Not a block parent?";
  } break;
  default:
    qDebug() << "Crop NYI";
    break;
  }
}

GfxImageItem::DragType GfxImageItem::dragTypeForPoint(QPointF p) {
  double x = p.x();
  double y = p.y();
  double w = boundingRect().width();
  double h = boundingRect().height();
  if (x/w < .25 && y/h < .25)
    return ResizeTopLeft;
  else if (x/w < .25 && y/h > .75)
    return ResizeBottomLeft;
  else if (x/w > .75 && y/h < .25)
    return ResizeTopRight;
  else if (x/w > .75 && y/h > .75)
    return ResizeBottomRight;
  else if (x/w < .15)
    return CropLeft;
  else if (x/w > .85)
    return CropRight;
  else if (y/h < .15)
    return CropTop;
  else if (y/h > .85)
    return CropBottom;
  else
    return Move;
}

Qt::CursorShape GfxImageItem::cursorForDragType(GfxImageItem::DragType dt) {
  switch (dt) {
  case None: case Move:
    return Qt::ArrowCursor;
  case ResizeTopLeft: case ResizeBottomRight:
    return Qt::SizeFDiagCursor;
  case ResizeTopRight: case ResizeBottomLeft:
    return Qt::SizeBDiagCursor;
  case CropLeft: case CropRight:
    return Qt::SizeHorCursor;    
  case CropTop: case CropBottom:
    return Qt::SizeVerCursor;    
  }
}

void GfxImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  Qt::CursorShape newCursor = Qt::ArrowCursor;
  double x = e->pos().x();
  double y = e->pos().y();
  double w = boundingRect().width();
  double h = boundingRect().height();
  if (e->modifiers() & GfxBlockItem::moveModifiers()) 
    newCursor = cursorForDragType(dragTypeForPoint(e->pos()));
  if (newCursor != oldCursor) {
    setCursor(QCursor(newCursor));
    oldCursor = newCursor;
  }
  e->accept();
}

