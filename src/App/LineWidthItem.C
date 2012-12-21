// LineWidthItem.C

#include "LineWidthItem.H"
#include <QPainter>

LineWidthItem::LineWidthItem(double lw): lw(lw), c("black") {
}

LineWidthItem::~LineWidthItem() {
}

void LineWidthItem::paintContents(QPainter *p) {
  p->setPen(QPen(c, lw));
  p->setBrush(Qt::NoBrush);
  p->drawLine(12, 24, 20, 8); // or something like that
}

void LineWidthItem::setColor(QColor c1) {
  c = c1;
  update();
}


void LineWidthItem::setLineWidth(double lw1) {
  lw = lw1;
  update();
}

