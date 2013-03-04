// ToolItem.C

#include "ToolItem.H"
#include "Toolbar.H"
#include <QSvgRenderer>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#define TOOLSIZE 31.5
#define TOOLRAD 3.0
#define HOVERDX 1.5
#define HOVERDX1 0.5
#define SHRINK 1

ToolItem::ToolItem(): QGraphicsObject() {
  svg = 0;
  sel = false;
  hov = false;
  setAcceptHoverEvents(true);
}

ToolItem::~ToolItem() {
}

void ToolItem::setSvg(QString filename) {
  if (svg)
    delete svg;
  svg = new QSvgRenderer(filename, this);
  update();
}

bool ToolItem::isSelected() const {
  return sel;
}

void ToolItem::setSelected(bool s) {
  sel = s;
  update();
}

QRectF ToolItem::boundingRect() const {
  return QRectF(0, 0, TOOLSIZE, TOOLSIZE);
}

void ToolItem::paintContents(QPainter *p) {
  if (svg) 
    svg->render(p, boundingRect());
}

void ToolItem::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) {
  QColor blk("black");
  blk.setAlphaF(0.25);
  QColor wht("white");
  wht.setAlphaF(0.5);

  if (sel) {  
    p->setPen(Qt::NoPen);
    p->setBrush(blk);
    p->drawRoundedRect(QRectF(0, 0, TOOLSIZE, TOOLSIZE), TOOLRAD, TOOLRAD);

    QColor grn("green");
    grn.setAlphaF(0.75);
    p->setBrush(QBrush(Qt::NoBrush));
    p->setPen(QPen(grn, 4));
    p->drawRoundedRect(QRectF(2, 2, TOOLSIZE-4, TOOLSIZE-4), TOOLRAD, TOOLRAD);

    p->setPen(Qt::NoPen);
    p->setBrush(wht);
    p->drawRoundedRect(QRectF(0, 0, TOOLSIZE-HOVERDX, TOOLSIZE-HOVERDX),
		       TOOLRAD, TOOLRAD);
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(blk);
    p->drawRoundedRect(QRectF(SHRINK, SHRINK,
			      TOOLSIZE-2*SHRINK, TOOLSIZE-2*SHRINK),
		       TOOLRAD, TOOLRAD);
    
    p->setBrush(wht);
    if (hov) 
      p->drawRoundedRect(QRectF(SHRINK+HOVERDX, SHRINK+HOVERDX,
				TOOLSIZE-2*SHRINK-HOVERDX,
				TOOLSIZE-2*SHRINK-HOVERDX),
			 TOOLRAD, TOOLRAD);
    else 
      p->drawRoundedRect(QRectF(SHRINK, SHRINK,
				TOOLSIZE-2*SHRINK-HOVERDX,
				TOOLSIZE-2*SHRINK-HOVERDX),
			 TOOLRAD, TOOLRAD);

    if (hov)
      p->translate(HOVERDX1, HOVERDX1);
  }    

  paintContents(p);
}

void ToolItem::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  hov = true;
  update();
}

void ToolItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  hov = false;
  update();
}

void ToolItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  switch (e->button()) {
  case Qt::LeftButton:
    emit leftClick();
    e->accept();
    break;
  case Qt::RightButton:
    emit rightClick();
    e->accept();
    break;
  default:
    e->ignore();
    break;
  }
}
