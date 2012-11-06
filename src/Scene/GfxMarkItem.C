// GfxMarkItem.C

#include "GfxMarkItem.H"
#include <QPainter>
#include <QPen>
#include <QBrush>

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
  switch (shp) {
  case GfxMarkData::SolidCircle:
  case GfxMarkData::SolidSquare:
    pen = QPen(Qt::NoPen);
    brush = QBrush(c);
    break;
  default:
    pen = QPen(c, 1.0);
    brush = QBrush(Qt::NoBrush);
    break;
  }

  p->setPen(pen);
  p->setBrush(brush);
  
  switch (shp) {
  case GfxMarkData::SolidCircle:
  case GfxMarkData::OpenCircle:
  case GfxMarkData::DotCircle:
    p->drawEllipse(p0.x()-siz, p0.y()-siz, 2*siz, 2*siz);
    break;
  case GfxMarkData::SolidSquare:
  case GfxMarkData::OpenSquare:
  case GfxMarkData::DotSquare:
    p->drawEllipse(p0.x()-.8*siz, p0.y()-.8*siz, 1.6*siz, 1.6*siz);
    break;
  case GfxMarkData::Cross:
    p->drawLine(p0.x()-.75*siz, p0.y()-.75*siz, p0.x()+.75*siz, p0.y()+.75*siz);
    p->drawLine(p0.x()-.75*siz, p0.y()+.75*siz, p0.x()+.75*siz, p0.y()-.75*siz);
  case GfxMarkData::Plus:
    p->drawLine(p0.x()-siz, p0.y(), p0.x()+siz, p0.y());
    p->drawLine(p0.x(), p0.x()-siz, p0.x(), p0.y()+siz);
    break;
  }
  if (shp==GfxMarkData::DotCircle 
      || shp==GfxMarkData::DotSquare) {
    pen.setWidth(2);
    p->setPen(pen);
    p->drawPoint(p0);
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
  
