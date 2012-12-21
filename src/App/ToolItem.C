// ToolItem.C

#include "ToolItem.H"
#include "Toolbar.H"
#include <QSvgRenderer>
#include <QGraphicsSceneMouseEvent>

#define TOOLSIZE 36.0

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
  p->setPen(Qt::NoPen);
  QColor blk("black");
  blk.setAlphaF(0.25);
  QColor wht("white");
  wht.setAlphaF(0.25);
  if (hov) {
    p->setBrush(blk);
    p->drawRoundedRect(QRectF(0, 0, TOOLSIZE-1, TOOLSIZE-1), 3, 3);
    p->setBrush(wht);
    p->drawRoundedRect(QRectF(1, 1, TOOLSIZE-1, TOOLSIZE-1), 3, 3);
  } else {
    p->setBrush(blk);
    p->drawRoundedRect(QRectF(1, 1, TOOLSIZE-1, TOOLSIZE-1), 3, 3);
    p->setBrush(wht);
    p->drawRoundedRect(QRectF(0, 0, TOOLSIZE-1, TOOLSIZE-1), 3, 3);
  }    
  if (sel) {
    QColor grn("green");
    grn.setAlphaF(0.25);
    p->setBrush(QBrush(Qt::NoBrush));
    p->setPen(QPen(grn, 4));
    p->drawRoundedRect(QRectF(2, 2, TOOLSIZE-4, TOOLSIZE-4), 3, 3);
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
