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
#include "PreliminaryLine.H"

GfxBlockItem::GfxBlockItem(GfxBlockData *data, PageScene *parent):
  BlockItem(data, parent),
  data_(data) {

  dragLine = 0;
  
  foreach (GfxData *d, data->gfx()) 
    addChild(create(d, this));

  setPos(Style::defaultStyle()["margin-left"].toDouble(), 0);
  setCursor(defaultCursor());
}

GfxBlockItem::~GfxBlockItem() {
}

GfxBlockData *GfxBlockItem::data() {
  return data_;
}

Item *GfxBlockItem::newImage(QImage img, QUrl const *src, QPointF xy) {
  // this needs some work
  Q_ASSERT(data()->book());
  Q_ASSERT(data()->resMgr());
  QString resName = data()->resMgr()->import(img, src);
  qDebug() << "GfxBlockItem::newImage" << resName << xy;
  
  GfxImageData *gid = new GfxImageData(resName, img);
  gid->setPos(QPointF(18, 18)); // should use passed xy?
  data()->addGfx(gid);
  GfxImageItem *gii = new GfxImageItem(gid, this);
  addChild(gii);
  qDebug() << "  new image: bbox: "
	   << gii->mapRectToParent(gii->boundingRect());
  sizeToFit();

  return gii;
}

Item *GfxBlockItem::newNote(QPointF p0) {
  qDebug() << "GBI:newNote" << p0;
  GfxNoteData *d = new GfxNoteData();
  d->setPos(p0);
  d->setLineLengthToZero();
  data_ -> addGfx(d);
  
  GfxNoteItem *i = new GfxNoteItem(d, this);
  addChild(i);
  sizeToFit();
  i->setFocus();
  return i;
}

Item *GfxBlockItem::newNote(QPointF p0, QPointF p1) {
  qDebug() << "GBI:newNote" << p0 << p1;
  GfxNoteData *d = new GfxNoteData();
  d->setPos(p0);
  d->setEndPoint(p1);
  data_ -> addGfx(d);
  GfxNoteItem *i = new GfxNoteItem(d, this);
  addChild(i);
  sizeToFit();
  i->setFocus();
  return i;
}

double GfxBlockItem::availableWidth() const {
  Style const &style(Style::defaultStyle());
  return style["page-width"].toDouble() -
    style["margin-left"].toDouble() -
    style["margin-right"].toDouble();
}

void GfxBlockItem::childGeometryChanged() {
  sizeToFit();
}

void GfxBlockItem::sizeToFit() {
  qDebug() << "GfxBlockItem::sizeToFit" << this;
  prepareGeometryChange();
  checkVbox();
  data_->setRef(-netBoundingRect().topLeft()); // this has the intended
  // side effect of updating the boundingRectCache
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

void GfxBlockItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxBlockItem::mouseMove" << e->pos();
  if (!dragLine) {
    qDebug() << "GfxBlockItem::mouseMove: No dragLine!?";
    return;
  }
  dragLine->updateEnd(e->pos());
  e->accept();
}

void GfxBlockItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxBlockItem::mousePress" << e->pos();
  if (dragLine) {
    qDebug() << "GfxBlockItem::mousePressEvent: dragLine pre-existed !?";
    e->ignore();
    return;
  }
  dragLine = new PreliminaryLine(e->pos(), this);
  e->accept();
}

void GfxBlockItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxBlockItem::mouseRelease" << e->pos();
  if (!dragLine) {
    qDebug() << "GfxBlockItem::mouseRelease: No dragLine!?";
    return;
  }

  if (dragLine->isLine()) 
    newNote(dragLine->startPoint(), dragLine->endPoint());
  else
    newNote(dragLine->startPoint());

  delete dragLine;
  dragLine = 0;

  e->accept();
}

