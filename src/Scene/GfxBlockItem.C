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
#include <math.h>
#include <QGraphicsSceneMouseEvent>

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

double GfxBlockItem::availableWidth() const {
  Style const &style(Style::defaultStyle());
  return style["page-width"].toDouble() -
    style["margin-left"].toDouble() -
    style["margin-right"].toDouble();
}

void GfxBlockItem::growToFit() {
  checkVbox();
  data_->setRef(-netBoundingRect().topLeft());
}

QRectF GfxBlockItem::netBoundingRect() const {
  QRectF b = QRectF(0, 0, availableWidth(), 72);
  foreach (QGraphicsItem *i, items_) {
    QRectF bb = i->mapRectToParent(i->boundingRect());
     if (dynamic_cast<GfxImageItem*>(i))
       bb.adjust(0, -18, 0, 18);
     else
       bb.adjust(0, -5, 0, 5);
    b |= bb;
  }
  return b;
}

void GfxBlockItem::paint(QPainter *p,
			 const QStyleOptionGraphicsItem *o,
			 QWidget *w) {
  // paint background grid; items draw themselves  
  Style const &style(Style::defaultStyle());
  QRectF bb = netBoundingRect();

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
}

void GfxBlockItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxBlockItem::mousePress" << e->pos();
  e->ignore();
}

void GfxBlockItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "GfxBlockItem::mouseRelease" << e->pos();
}

Qt::KeyboardModifiers GfxBlockItem::moveModifiers() {
  return Qt::AltModifier | Qt::MetaModifier | Qt::GroupSwitchModifier;
}

Qt::MouseButton GfxBlockItem::moveButton() {
  return Qt::LeftButton;
}
