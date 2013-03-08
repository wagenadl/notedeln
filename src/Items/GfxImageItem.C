// GfxImageItem.C

#include "GfxImageItem.H"
#include "GfxImageData.H"
#include "PageScene.H"
#include "Mode.H"
#include "GfxNoteData.H"
#include "GfxNoteItem.H"
#include "GfxMarkItem.H"
#include "GfxSketchItem.H"
#include "BlockItem.H"

#include <QProcess>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include "ResManager.H"
#include <QCursor>
#include <math.h>

static Item::Creator<GfxImageData, GfxImageItem> c("gfximage");

GfxImageItem::GfxImageItem(GfxImageData *data, Item *parent):
  Item(data, parent) {
  pixmap = new QGraphicsPixmapItem(this);
  pixmap->setAcceptedMouseButtons(0);

  dragType = None;

  // get the image, crop it, etc.
  ResManager *resmgr = data->resManager();
  if (!resmgr) {
    qDebug() << "GfxImageItem: no resource manager";
    return;
  }
  Resource *res = resmgr->byTag(data->resName());
  if (!res) {
    qDebug() << "GfxImageItem: missing resource" << data->resName();
    return;
  }
  if (!image.load(res->archivePath())) {
    qDebug() << "GfxImageItem: image load failed for " << data->resName()
	     << res->archivePath();
    return;
  }
  pixmap->setPixmap(QPixmap::fromImage(image.copy(data->cropRect().toRect())));
  setScale(data->scale());
  setPos(data->pos());
  pixmap->setPos(data->cropRect().topLeft());
  pixmap->setTransformationMode(Qt::SmoothTransformation);

  foreach (GfxData *gd, data->children<GfxData>()) {
    Item *i = create(gd, this);
    i->setScale(1./data->scale());
  }

  oldCursor = Qt::ArrowCursor;
  setCursor(defaultCursor());
}

GfxImageItem::~GfxImageItem() {
}

QPointF GfxImageItem::moveDelta(QGraphicsSceneMouseEvent *e) {
  QPointF delta = mapToParent(e->pos()) - dragStart;
  return delta;
}

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
    setPos(data()->pos() + moveDelta(e));
    break;
  case ResizeTopLeft: {
    QPointF xy0 = cropStart.bottomRight();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().bottomRight()));
  } break;
  case ResizeTopRight: {
    QPointF xy0 = cropStart.bottomLeft();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().bottomLeft()));
  } break;
  case ResizeBottomLeft: {
    QPointF xy0 = cropStart.topRight();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().topRight()));
  } break;
  case ResizeBottomRight: {
    QPointF xy0 = cropStart.topLeft();
    double diag0 = euclideanLength(dragStart - xy0);
    double diag1 = euclideanLength(ppos - xy0);
    setScale(data()->scale() * diag1/diag0);
    setPos(pos() + xy0 - mapToParent(imageBoundingRect().topLeft()));
  } break;
  case CropLeft: {
    double dx = moveDelta(e).x();
    double x0 = cropStart.left();
    double x = x0 + dx;
    if (x < imStart.left())
      x = imStart.left();
    else if (x > cropStart.right())
      x = cropStart.right();
    dx = x - x0;
    QRectF cr = cropStart; cr.setLeft(x);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
    pixmap->setPos(dragCrop.topLeft());
  } break;
  case CropRight: {
    double dx = moveDelta(e).x();
    double x0 = cropStart.right();
    double x = x0 + dx;
    if (x < cropStart.left())
      x = cropStart.left();
    else if (x > imStart.right())
      x = imStart.right();
    dx = x - x0;
    QRectF cr = cropStart; cr.setRight(x);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
  } break;
  case CropTop: {
    double dy = moveDelta(e).y();
    double y0 = cropStart.top();
    double y = y0 + dy;
    if (y < imStart.top())
      y = imStart.top();
    else if (y > cropStart.bottom())
      y = cropStart.bottom();
    dy = y - y0;
    QRectF cr = cropStart; cr.setTop(y);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
    pixmap->setPos(dragCrop.topLeft());
  } break;
  case CropBottom: {
    double dy = moveDelta(e).y();
    double y0 = cropStart.bottom();
    double y = y0 + dy;
    if (y < cropStart.top())
      y = cropStart.top();
    else if (y > imStart.bottom())
      y = imStart.bottom();
    dy = y - y0;
    QRectF cr = cropStart; cr.setBottom(y);
    dragCrop = mapRectFromParent(cr).toRect();
    pixmap->setPixmap(QPixmap::fromImage(image.copy(dragCrop)));
  } break;
  }
}

void GfxImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  Resource *r = data()->resManager()->byTag(data()->resName());
  if (!r) {
    qDebug() << "GfxImageItem: double click: no resource";
  } else {
    QStringList args;
    if (e->modifiers() & Qt::ShiftModifier)
      args << r->sourceURL().toString();
    else
      args << r->archivePath();
    bool ok = QProcess::startDetached("gnome-open", args);
    if (!ok)
      qDebug() << "GfxImageItem: Failed to start external process 'gnome-open'";
  }
  e->accept();
}

void GfxImageItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  bool take = false;
  if (e->modifiers() & Qt::ControlModifier) {
    //take = true; // we will not process this, but consider a double click
  } else if (isWritable() && e->button()==Qt::LeftButton) {
    switch (mode()->mode()) {
    case Mode::MoveResize:
      dragType = dragTypeForPoint(e->pos());
      dragStart = mapToParent(e->pos());
      cropStart = mapRectToParent(data()->cropRect());
      imStart = mapRectToParent(QRectF(QPointF(0,0), data()->size()));
      dragCrop = data()->cropRect().toRect();
      if (ancestralBlock())
	ancestralBlock()->lockBounds();
      else
	qDebug() << "GfxImageItem: no parent";
      take = true;
      break;
    case Mode::Annotate: {
      GfxNoteItem *gni = createNote(e->pos(), !isWritable());
      gni->setScale(1./data()->scale());
      take = true;
    } break;
    case Mode::Mark: {
      GfxMarkItem *mi = GfxMarkItem::newMark(e->pos(), this);
      mi->setScale(1./data()->scale());
      take = true;
    } break;
    case Mode::Freehand: {
      GfxSketchItem *mi = GfxSketchItem::newSketch(e->pos(), this);
      mi->setScale(1./data()->scale());
      mi->build();
      take = true;
    } break;
    default:
      break;
    }
  } else {
    if (e->button()==Qt::LeftButton && mode()->mode()==Mode::Annotate) {
      GfxNoteItem *gni = createNote(e->pos(), true);
      gni->setScale(1./data()->scale());
      take = true;
    }
  }
  
  if (take) {
    e->accept();
  } else {
    e->ignore();
  }
}

void GfxImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  switch (dragType) {
  case None:
    qDebug() << "Nonmove!?";
    break;
  case Move: 
    data()->setPos(pos());
    break;
  case ResizeTopLeft: case ResizeTopRight:
  case ResizeBottomLeft: case ResizeBottomRight: 
    data()->setPos(pos());
    data()->setScale(scale());
    break;
  case CropLeft: case CropRight:
  case CropTop: case CropBottom:
    data()->setCropRect(dragCrop); // round to integers
    break;
  }
  if (ancestralBlock()) {
    ancestralBlock()->unlockBounds();
  }
  e->accept();
}

GfxImageItem::DragType GfxImageItem::dragTypeForPoint(QPointF p) {
  p -= data()->cropRect().topLeft();
  double x = p.x();
  double y = p.y();
  double w = imageBoundingRect().width();
  double h = imageBoundingRect().height();
  //  qDebug() << "GII: DragTypeForPoint" << x << y << w << h;
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
    return Qt::SizeAllCursor;
  case ResizeTopLeft: case ResizeBottomRight:
    return Qt::SizeFDiagCursor;
  case ResizeTopRight: case ResizeBottomLeft:
    return Qt::SizeBDiagCursor;
  case CropLeft: case CropRight:
    return Qt::SplitHCursor;
  case CropTop: case CropBottom:
    return Qt::SplitVCursor;
  }
  return defaultCursor(); // this statement will not be reached
}

void GfxImageItem::setCursor(Qt::CursorShape newCursor) {
  if (newCursor==oldCursor)
    return;
  
  QGraphicsObject::setCursor(newCursor);
  oldCursor = newCursor;
}

void GfxImageItem::modeChange(Mode::M m) {
  if (m==Mode::MoveResize)
    setCursor(cursorForDragType(dragTypeForPoint(cursorPos)));
  else 
    setCursor(Qt::CrossCursor);
}

void GfxImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  if (mode()->mode()==Mode::MoveResize)
    setCursor(cursorForDragType(dragTypeForPoint(cursorPos)));
  else
    setCursor(Qt::CrossCursor);
  e->accept();
}

QRectF GfxImageItem::imageBoundingRect() const {
  return pixmap->mapRectToParent(pixmap->boundingRect());
}

QRectF GfxImageItem::boundingRect() const {
  return imageBoundingRect().adjusted(0, -18, 0, 18);
}

void GfxImageItem::makeWritable() {
  connect(mode(), SIGNAL(modeChanged(Mode::M)),
	  SLOT(modeChange(Mode::M)));
  //  qDebug() << "GII:MakeWritable";
  Item::makeWritable();
  setAcceptHoverEvents(true);
}

void GfxImageItem::setScale(double s) {
  QGraphicsObject::setScale(s);
  foreach (Item *i, allChildren())
    i->setScale(1./s);
}

void GfxImageItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}
