// GfxBlockItem.C

#include "GfxBlockItem.H"
#include "GfxBlockData.H"
#include "Style.H"
#include <QPainter>
#include "GfxItemFactory.H"
#include <QDebug>
#include "ResourceManager.H"
#include "GfxImageData.H"
#include "GfxImageItem.H"

GfxBlockItem::GfxBlockItem(GfxBlockData *data, PageScene *parent):
  BlockItem(data, parent),
  data_(data) {
  // let's get stuff from the data...
  qDebug() << "GfxBlockItem!";
  foreach (GfxData *d, data->gfx()) {
    qDebug() << "  d="<<d;
    items_.append(GfxItemFactory::create(d, this));
  }
  qDebug() <<"  items="<<items_;

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

GfxImageItem *GfxBlockItem::newImage(QImage img, QUrl const *src, QPointF xy) {
  // this needs some work
  Q_ASSERT(data()->book());
  Q_ASSERT(data()->resMgr());
  QString resName = data()->resMgr()->import(img, src);
  qDebug() << "GfxBlockItem::newImage" << resName << xy;
  if (true || isEmpty()) {
    // let's just place it
    GfxImageData *gid = new GfxImageData(resName, img);
    gid->setXY(QPointF(18, 18));
    data()->addGfx(gid);
    GfxImageItem *gii = new GfxImageItem(gid, this);
    items_.append(gii);
    qDebug() << "  new image: bbox: "
	     << gii->mapRectToParent(gii->boundingRect());
    growToFit();
  } else {
    // should use passed position info?
  }
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

void GfxBlockItem::growToFit() {
  double h0 = height(); // current height
  QRectF b = netBoundingRect();
  double h1 = b.height(); // contents height
  qDebug() << "GBI:growtofit h0="<<h0 << " h1="<<h1;
  if (h1>h0) {
    data_->setHeight(h1); // make us be larger
    checkVbox();
  }
}

QRectF GfxBlockItem::netBoundingRect() const {
  QRectF b = QRectF(0, 0, width(), height());
  foreach (QGraphicsItem *i, items_) {
    QRectF bb = i->mapRectToParent(i->boundingRect());
    if (dynamic_cast<GfxImageItem*>(i))
      bb.adjust(-18, -18, 18, 18);
    else
      bb.adjust(-5, -5, 5, 5);
    b |= bb;
  }
  return b;
}

void GfxBlockItem::paint(QPainter *p,
			 const QStyleOptionGraphicsItem *o,
			 QWidget *w) {
  // paint background grid; items draw themselves  
  Style const &style(Style::defaultStyle());
  p->setPen(QPen(QBrush(QColor(style["canvas-grid-color"].toString())),
		 style["canvas-grid-line-width"].toDouble(),
		 Qt::SolidLine,
		 Qt::FlatCap));
  double dx = style["canvas-grid-spacing"].toDouble();
  QRectF bb = netBoundingRect();
  for (double x = bb.left()+dx/2; x<bb.right(); x+=dx)
    p->drawLine(x, bb.top(), x, bb.bottom());
  for (double y = bb.top()+dx/2; y<bb.bottom(); y+=dx)
    p->drawLine(bb.left(), y, bb.right(), y);
}
