// GfxImageItem.C

#include "GfxImageItem.H"
#include "GfxItemFactory.H"
#include "GfxImageData.H"
#include "PageScene.H"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include "ResourceManager.H"
#include <QCursor>
#include <math.h>

static Item::Creator<GfxImageData, GfxImageItem> c("gfximage");

GfxImageItem::GfxImageItem(GfxImageData *data, Item *parent):
  QObject(Item::obj(parent)),
  QGraphicsPixmapItem(Item::gi(parent)),
  Item(data, this),
  data(data) {
  if (!data) {
    qDebug() << "GfxImageItem constructed w/o data";
    return;
  }

  dragType = None;
  moveModPressed = false;

  // get the image, crop it, etc.
  ResourceManager *resmgr = data->resMgr();
  if (!resmgr) {
    qDebug() << "GfxImageItem: no resource manager";
    return;
  }
  if (!image.load(resmgr->path(data->resName()))) {
    qDebug() << "GfxImageItem: image load failed for " << data->resName()
	     << resmgr->path(data->resName());
    return;
  }
  setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  setScale(data->scale());
  setPos(data->pos());
  setAcceptHoverEvents(true);
  oldCursor = Qt::ArrowCursor;
  setCursor(GfxBlockItem::defaultCursor());
  PageScene *s = dynamic_cast<PageScene*>(scene());
  if (s) 
    connect(s, SIGNAL(modifiersChanged(Qt::KeyboardModifiers)),
	    SLOT(modifierChange(Qt::KeyboardModifiers)));
  else
    qDebug() << "GfxImageItem: no page -> keyboard modifiers will be ignored";
}

GfxImageItem::~GfxImageItem() {
}

//void GfxImageItem::showCroppedAreas() {
//  QPixmap pm(QPixmap::fromImage(image));
//  setPixmap(pm);
//  setOffset(-data->cropLeft(), -data->cropTop());
//}
//
//void GfxImageItem::hideCroppedAreas() {
//  setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
//  setOffset(0, 0);
//}

QPointF GfxImageItem::moveDelta(QGraphicsSceneMouseEvent *e) {
  QPointF delta = mapToParent(e->pos()) - dragStart;
  return delta;
}

//static double operator*(QPointF a, QPointF b) {
//  return a.x()*b.x() + a.y()*b.y();
//}

static double euclideanLength(QPointF a) {
  return sqrt(a.x()*a.x() + a.y()*a.y());
}

void GfxImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  QPointF ppos = mapToParent(e->pos());
  switch (dragType) {
  case None:
    qDebug() << " Nonmove!?";
    break;
  case Move:
    setPos(data->pos() + moveDelta(e));
    break;
  case ResizeTopLeft: {
    QPointF xy0 = data->blockRect().bottomRight();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().bottomRight()));
  } break;
  case ResizeTopRight: {
    QPointF xy0 = data->blockRect().bottomLeft();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().bottomLeft()));
  } break;
  case ResizeBottomLeft: {
    QPointF xy0 = data->blockRect().topRight();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().topRight()));
  } break;
  case ResizeBottomRight: {
    QPointF xy0 = data->blockRect().topLeft();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data->scale() * diag1/diag0);
  } break;
  case CropLeft: {
    double dx = moveDelta(e).x();
    dragCrop = data->cropRect();
    double x0 = dragCrop.left();
    double x = x0 + dx;
    if (x < 0)
      x = 0;
    else if (x > dragCrop.right())
      x = dragCrop.right();
    dx = x - x0;
    dragCrop.setLeft(x);
    setPixmap(QPixmap::fromImage(image.copy(dragCrop.toRect())));
    setPos(pos() + data->blockRect().bottomRight()
	   - mapRectToParent(imageBoundingRect()).bottomRight());
  } break;
  case CropRight: {
    double dx = moveDelta(e).x();
    dragCrop = data->cropRect();
    double x0 = dragCrop.right();
    double x = x0 + dx;
    if (x < dragCrop.left())
      x = dragCrop.left();
    else if (x > data->width())
      x = data->width();
    dx = x - x0;
    dragCrop.setRight(x);
    setPixmap(QPixmap::fromImage(image.copy(dragCrop.toRect())));
  } break;
  case CropTop: {
    double dy = moveDelta(e).y();
    dragCrop = data->cropRect();
    double y0 = dragCrop.top();
    double y = y0 + dy;
    if (y < 0)
      y = 0;
    else if (y > dragCrop.bottom())
      y = dragCrop.bottom();
    dy = y - y0;
    dragCrop.setTop(y);
    setPixmap(QPixmap::fromImage(image.copy(dragCrop.toRect())));
    setPos(pos() + data->blockRect().bottomRight()
	   - mapRectToParent(imageBoundingRect()).bottomRight());
  } break;
  case CropBottom: {
    double dy = moveDelta(e).y();
    dragCrop = data->cropRect();
    double y0 = dragCrop.bottom();
    double y = y0 + dy;
    if (y < dragCrop.top())
      y = dragCrop.top();
    else if (y > data->height())
      y = data->height();
    dy = y - y0;
    dragCrop.setBottom(y);
    setPixmap(QPixmap::fromImage(image.copy(dragCrop.toRect())));
  } break;
  }
}

void GfxImageItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  if (moveModPressed) { // if (e->modifiers() & GfxBlockItem::moveModifiers())
    dragType = dragTypeForPoint(e->pos());
    dragStart = mapToParent(e->pos());
    dragCrop = data->cropRect();
    if (itemParent())
      itemParent()->lockBounds();
    else
      qDebug() << "GfxImageItem: no parent";
    e->accept();
  } else {
    e->ignore(); // pass to parent
  }
}

void GfxImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  switch (dragType) {
  case None:
    qDebug() << "Nonmove!?";
    break;
  case Move: 
    data->setPos(pos());
    break;
  case ResizeTopLeft: case ResizeTopRight:
  case ResizeBottomLeft: case ResizeBottomRight: 
    data->setPos(pos());
    data->setScale(scale());
    break;
  case CropLeft: case CropRight:
  case CropTop: case CropBottom:
    data->setPos(pos());
    data->setCropRect(dragCrop.toRect()); // round to integers
    break;
  }
  if (itemParent()) {
    itemParent()->unlockBounds();
    itemParent()->childGeometryChanged();
  }
  e->accept();
}

GfxImageItem::DragType GfxImageItem::dragTypeForPoint(QPointF p) {
  double x = p.x();
  double y = p.y();
  double w = imageBoundingRect().width();
  double h = imageBoundingRect().height();
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
  case None:
    return Qt::ArrowCursor;
  case Move:
    return Qt::ArrowCursor;
  case ResizeTopLeft: case ResizeBottomRight:
    return Qt::SizeFDiagCursor;
  case ResizeTopRight: case ResizeBottomLeft:
    return Qt::SizeBDiagCursor;
  case CropLeft: case CropRight:
    return Qt::SplitHCursor;
  case CropTop: case CropBottom:
    return Qt::SplitVCursor;
  }
  return GfxBlockItem::defaultCursor(); // this statement will not be reached
}

void GfxImageItem::setCursor(Qt::CursorShape newCursor) {
  if (newCursor==oldCursor)
    return;
  
  QGraphicsPixmapItem::setCursor(newCursor);
  oldCursor = newCursor;
}

void GfxImageItem::modifierChange(Qt::KeyboardModifiers m) {
  moveModPressed = m & GfxBlockItem::moveModifiers();
  if (moveModPressed)
    setCursor(cursorForDragType(dragTypeForPoint(cursorPos)));
  else 
   setCursor(GfxBlockItem::defaultCursor());
}

void GfxImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  if (moveModPressed)
    setCursor(cursorForDragType(dragTypeForPoint(cursorPos)));
  else
    setCursor(GfxBlockItem::defaultCursor());
  e->accept();
}

QRectF GfxImageItem::imageBoundingRect() const {
  return QGraphicsPixmapItem::boundingRect();
}

QRectF GfxImageItem::boundingRect() const {
  return imageBoundingRect().adjusted(0, -18, 0, 18);
}
