// GfxBlockItem.C

#include "GfxBlockItem.H"
#include "GfxBlockData.H"
#include "Style.H"
#include <QPainter>
#include "GfxItemFactory.H"

GfxBlockItem::GfxBlockItem(GfxBlockData *data, PageScene *parent):
  BlockItem(data, parent),
  data_(data) {
  // let's get stuff from the data...
  foreach (GfxData *d, data->gfx())
    items_.append(GfxItemFactory::create(d, this));

  setPos(Style::defaultStyle()["margin-left"].toDouble(), 0);
}

GfxBlockItem::~GfxBlockItem() {
}

GfxBlockData *GfxBlockItem::data() {
  return data_;
}

QList<class QGraphicsItem *> const &GfxBlockItem::gfx() {
  return items_;
}

bool GfxBlockItem::isEmpty() const {
  return items_.isEmpty();
}

GfxImageItem *GfxBlockItem::newImage(QString resName, QPointF xy) {
  // this needs some work
  checkVbox();
  return 0;
}

double GfxBlockItem::width() const {
  Style const &style(Style::defaultStyle());
  return style["page-width"].toDouble() -
    style["margin-left"].toDouble() -
    style["margin-right"].toDouble();
}

double GfxBlockItem::height() const {
  return data_ ? data_->height() : 0;
}


QRectF GfxBlockItem::netBoundingRect() const {
  QRectF b = QRectF(0, 0, width(), height());
  foreach (QGraphicsItem *i, items_)
    b |= i->mapRectToParent(i->boundingRect());
  return b;
}

void GfxBlockItem::paint(QPainter *p,
			 const QStyleOptionGraphicsItem *o,
			 QWidget *w) {
  // paint background grid; items draw themselves  
  Style const &style(Style::defaultStyle());
  p->setPen(QPen(QBrush(QColor(style["canvas-grid-color"].toString())),
		 style["canvas-grid-line-width"].toDouble()));
  double dx = style["canvas-grid-spacing"].toDouble();
  double ww = width();
  double hh = height();
  for (double x = dx; x<ww; x+=dx)
    p->drawLine(x, 0, x, hh);
  for (double y = dx; y<hh; y+=dx)
    p->drawLine(0, y, ww, y);
}
