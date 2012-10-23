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

GfxBlockItem::GfxBlockItem(GfxBlockData *data, PageScene *parent):
  BlockItem(data, parent),
  data_(data) {

  foreach (GfxData *d, data->gfx()) {
    Item *i = create(d, this);
    addChild(i);
  }

  setPos(Style::defaultStyle()["margin-left"].toDouble(), 0);
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
  GfxImageData *gid = new GfxImageData(resName, img);
  gid->setPos(QPointF(18, 18)); // should use passed xy?
  data()->addChild(gid);
  GfxImageItem *gii = new GfxImageItem(gid, this);
  addChild(gii);
  sizeToFit();
  return gii;
}

double GfxBlockItem::availableWidth() const {
  Style const &style(Style::defaultStyle());
  return style["page-width"].toDouble() -
    style["margin-left"].toDouble() -
    style["margin-right"].toDouble();
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
    foreach (Item *i, allChildren()) {
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
  Style const &style(Style::defaultStyle());
  QRectF bb = boundingRect();

  p->setPen(QPen(QBrush(QColor(style["canvas-grid-color"].toString())),
		 style["canvas-grid-line-width"].toDouble(),
		 Qt::SolidLine,
		 Qt::FlatCap));
  double dx = style["canvas-grid-spacing"].toDouble();
  drawGrid(p, bb, dx);

  p->setPen(QPen(QBrush(QColor(style["canvas-grid-major-color"].toString())),
		 style["canvas-grid-major-line-width"].toDouble(),
		 Qt::SolidLine,
		 Qt::FlatCap));
  dx *= style["canvas-grid-major-interval"].toInt();
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
  foreach (Item *i, allChildren())
    i->makeWritable();
}

void GfxBlockItem::modifierChange(Qt::KeyboardModifiers) {
  // this will only be called if we are writable
  if (moveModPressed())
    setCursor(Qt::ForbiddenCursor);
  else
    setCursor(Qt::CrossCursor);
}

