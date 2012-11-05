// GfxBlockItem.C

#include "GfxBlockItem.H"
#include "GfxBlockData.H"
#include "Style.H"
#include <QPainter>
#include <QDebug>
#include "ResourceManager.H"
#include "GfxImageData.H"
#include "GfxImageItem.H"
#include "GfxNoteData.H"
#include "GfxNoteItem.H"
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include "DragLine.H"

GfxBlockItem::GfxBlockItem(GfxBlockData *data, Item *parent):
  BlockItem(data, parent), data_(data) {

  foreach (GfxData *g, data->gfx()) 
    create(g, this);

  qDebug() << "GfxBlockItem" << this;
  setPos(style().real("margin-left"), 0);
  setCursor(defaultCursor());
}

GfxBlockItem::~GfxBlockItem() {
}

GfxBlockData *GfxBlockItem::data() {
  return data_;
}

Item *GfxBlockItem::newImage(QImage img, QUrl const *src, QPointF) {
  Q_ASSERT(data()->book());
  Q_ASSERT(data()->resMgr());
  QString resName = data()->resMgr()->import(img, src);
  GfxImageData *gid = new GfxImageData(resName, img, data());
  gid->setPos(QPointF(18, 18)); // should use passed xy?
  GfxImageItem *gii = new GfxImageItem(gid, this);
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
  // qDebug() << "GfxBlockItem::boundingRect" << this;
  QRectF bb(cachedBounds());
  if (bb.isNull()) {
    bb = QRectF(0, 0, availableWidth(), 72);
    // must do this here, because I cannot use our netBoundingRect...
    foreach (Item *i, itemChildren<Item>()) {
      if (!i->isExtraneous()) {
	// qDebug() << "  GBI" << this << ": Including child " << i;
	bb |= gi(i)->mapRectToParent(i->netBoundingRect());
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

  p->setPen(QPen(QBrush(style().color("canvas-grid-color")),
		 style().real("canvas-grid-line-width"),
		 Qt::SolidLine,
		 Qt::FlatCap));
  double dx = style().real("canvas-grid-spacing");
  drawGrid(p, bb, dx);

  p->setPen(QPen(QBrush(style().color("canvas-grid-major-color")),
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

