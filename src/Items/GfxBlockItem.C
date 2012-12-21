// GfxBlockItem.C

#include "ModSnooper.H"
#include "GfxBlockItem.H"
#include "GfxBlockData.H"
#include "Style.H"
#include <QPainter>
#include <QDebug>
#include "ResManager.H"
#include "GfxImageData.H"
#include "GfxImageItem.H"
#include "GfxNoteData.H"
#include "GfxNoteItem.H"
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include "DragLine.H"
#include "GfxMarkItem.H"
#include "GfxSketchItem.H"
#include "Assert.H"

GfxBlockItem::GfxBlockItem(GfxBlockData *data, Item *parent):
  BlockItem(data, parent), data_(data) {

  foreach (GfxData *g, data->gfx()) 
    create(g, this);

  setPos(style().real("margin-left"), 0);
  setCursor(defaultCursor());
}

GfxBlockItem::~GfxBlockItem() {
}

GfxBlockData *GfxBlockItem::data() {
  return data_;
}

static QPointF constrainPointToRect(QPointF p, QRectF rect) {
  if (p.x()<rect.left())
    p.setX(rect.left());
  else if (p.x()>rect.right())
    p.setX(rect.right());
  if (p.y()<rect.top())
    p.setY(rect.top());
  else if (p.y()>rect.bottom())
    p.setY(rect.bottom());
  return p;
}

Item *GfxBlockItem::newImage(QImage img, QUrl src, QPointF pos) {
  ASSERT(data()->book());
  ASSERT(data()->resManager());
  double maxW = availableWidth();
  double maxH = maxW;
  double scale = 1;
  if (scale*img.width()>maxW)
    scale = maxW/img.width();
  if (scale*img.height()>maxH)
    scale = maxH/img.height();
  if (itemChildren<Item>().isEmpty())
    pos = QPointF(18, 18);
  else
    pos -= QPointF(img.width(),img.height())*(scale/2);
  pos = constrainPointToRect(pos, boundingRect());
  Resource *res = data()->resManager()->importImage(img, src);
  QString resName = res->tag();
  GfxImageData *gid = new GfxImageData(resName, img, data());
  gid->setScale(scale);
  gid->setPos(pos);
  GfxImageItem *gii = new GfxImageItem(gid, this);
  gii->makeWritable();
  sizeToFit();
  return gii;
}

double GfxBlockItem::availableWidth() const {
  return style().real("page-width") -
    style().real("margin-left") -
    style().real("margin-right");
}

void GfxBlockItem::childGeometryChanged() {
  BlockItem::childGeometryChanged();
  sizeToFit();
}

void GfxBlockItem::sizeToFit() {
  prepareGeometryChange();
  checkVbox();
  data_->setRef(-netBoundingRect().topLeft());
}

QRectF GfxBlockItem::boundingRect() const {
  QRectF bb(cachedBounds());
  if (bb.isNull()) {
    bb = QRectF(0, 0, availableWidth(), 72);
    // must do this here, because I cannot use our netBoundingRect...
    foreach (Item *i, itemChildren<Item>()) {
      if (!i->isExtraneous()) {
	bb |= i->mapRectToParent(i->netBoundingRect());
      }
    }
  }
  bb.setLeft(0);
  bb.setWidth(availableWidth());
  return bb;
}
  
void GfxBlockItem::paint(QPainter *p,
			 const QStyleOptionGraphicsItem *,
			 QWidget *) {
  // paint background grid; items draw themselves  
  QRectF bb = boundingRect();
  QColor c(style().color("canvas-grid-color"));
  c.setAlphaF(style().real("canvas-grid-alpha"));
  p->setPen(QPen(c,
		 style().real("canvas-grid-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  double dx = style().real("canvas-grid-spacing");
  drawGrid(p, bb, dx);

  c = style().color("canvas-grid-major-color");
  c.setAlphaF(style().real("canvas-grid-major-alpha"));

  p->setPen(QPen(c,
		 style().real("canvas-grid-major-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  dx *= style().integer("canvas-grid-major-interval");
  if (dx)
    drawGrid(p, bb, dx);
}

void GfxBlockItem::drawGrid(QPainter *p, QRectF const &bb, double dx) {
  double x0 = dx*ceil(bb.left()/dx);
  double x1 = dx*floor(bb.right()/dx);
  double y0 = dx*ceil(bb.top()/dx);
  double y1 = dx*floor(bb.bottom()/dx);
  for (double x = x0; x<=x1+.001; x+=dx)
    p->drawLine(x, bb.top(), x, bb.bottom());
  for (double y = y0; y<=y1+.001; y+=dx)
    p->drawLine(bb.left(), y, bb.right(), y);
}

void GfxBlockItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  Qt::KeyboardModifiers mod = modSnooper()->keyboardModifiers();
  Qt::MouseButton but = e->button();
  bool take = false;
  if (but==Qt::LeftButton) {
    if (mod==0) {
      createNote(e->pos(), !data()->isRecent());
      take = true;
    } else if (mod & Qt::ControlModifier && data()->isRecent()) {
      GfxMarkItem::newMark(e->pos(), this);
      take = true;
    } else if (mod & Qt::ShiftModifier && data()->isRecent()) {
      GfxSketchItem *ski = GfxSketchItem::newSketch(e->pos(), this);
      ski->build();
      take = true;
    }
  }

  if (take) 
    e->accept();
  else
    BlockItem::mousePressEvent(e);
}

void GfxBlockItem::makeWritable() {
  BlockItem::makeWritable();
  setCursor(Qt::CrossCursor);
  acceptModifierChanges();

  // is it really right to make all children writable?
  foreach (Item *i, itemChildren<Item>())
    i->makeWritable();
}

void GfxBlockItem::modifierChange(Qt::KeyboardModifiers) {
  // this will only be called if we are writable
  if (moveModPressed())
    setCursor(Qt::ForbiddenCursor);
  else
    setCursor(Qt::CrossCursor);
}

