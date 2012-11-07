// GfxMarkItem.C

#include "GfxMarkItem.H"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <math.h>

static Item::Creator<GfxMarkData, GfxMarkItem> c("gfxmark");

GfxMarkItem::GfxMarkItem(GfxMarkData *data, Item *parent):
  QGraphicsObject(gi(parent)), Item(*this), d(data) {
  setPos(d->pos());
}

GfxMarkItem::~GfxMarkItem() {
}

GfxMarkData *GfxMarkItem::data() {
  return d;
}

QRectF GfxMarkItem::boundingRect() const {
  double s = d->size() + 1; // add +1 for line width
  return QRectF(-s,-s,s,s);
}

void GfxMarkItem::paint(QPainter *p,
			const QStyleOptionGraphicsItem *,
			QWidget *) {
  renderMark(QPointF(0,0),
	     d->color(), d->size(), d->shape(),
	     p);
}

void GfxMarkItem::renderMark(QPointF p0,
			     QColor c, double siz, GfxMarkData::Shape shp,
			     QPainter *p) {
  QPen pen;
  QBrush brush;
  QPointF delta(siz,siz);
  
  switch (shp) {
  case GfxMarkData::SolidCircle:
  case GfxMarkData::SolidSquare:
    pen = QPen(Qt::NoPen);
    brush = QBrush(c);
    break;
  default:
    pen = QPen(c);
    pen.setWidthF(sqrt(siz)/1.5);
    pen.setJoinStyle(Qt::MiterJoin);
    brush = QBrush(Qt::NoBrush);
    break;
  }

  p->setPen(pen);
  p->setBrush(brush);
  
  switch (shp) {
  case GfxMarkData::SolidCircle:
  case GfxMarkData::OpenCircle:
  case GfxMarkData::DotCircle:
    p->drawEllipse(QRectF(p0-delta, p0+delta));
    break;
  case GfxMarkData::SolidSquare:
  case GfxMarkData::OpenSquare:
  case GfxMarkData::DotSquare:
    p->drawRect(QRectF(p0-delta*.85, p0+delta*.85));
    break;
  case GfxMarkData::Cross:
    p->drawLine(p0 - QPointF(siz, siz)*.75, p0 + QPointF(siz, siz)*.75);
    p->drawLine(p0 - QPointF(siz, -siz)*.75, p0 + QPointF(siz, -siz)*.75);
    break;
  case GfxMarkData::Plus:
    p->drawLine(p0 - QPointF(siz, 0), p0 + QPointF(siz, 0));
    p->drawLine(p0 - QPointF(0, siz), p0 + QPointF(0, siz));
    break;
  }
  switch (shp) {
  case GfxMarkData::DotCircle:
    pen.setWidthF(sqrt(siz));
    pen.setCapStyle(Qt::RoundCap);
    p->setPen(pen);
    p->drawLine(p0-QPointF(1e-5,0), p0+QPointF(1e-5,0));
    break;
  case GfxMarkData::DotSquare:
    pen.setWidthF(sqrt(siz));
    pen.setCapStyle(Qt::SquareCap);
    p->setPen(pen);
    p->drawLine(p0-QPointF(1e-5,0), p0+QPointF(1e-5,0));
    break;
  default:
    break;
  }
}
  
GfxMarkItem *GfxMarkItem::newMark(QPointF p,
				  QColor c, double siz, GfxMarkData::Shape shp,
				  Item *parent) {
  GfxMarkData *gmd = new GfxMarkData(parent->data());
  gmd->setPos(p);
  gmd->setColor(c);
  gmd->setSize(siz);
  gmd->setShape(shp);
  GfxMarkItem *gmi = new GfxMarkItem(gmd, parent);
  return gmi;
}
  
