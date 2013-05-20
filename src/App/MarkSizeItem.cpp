// MarkSizeItem.C

#include "MarkSizeItem.H"
#include <QPainter>
#include "GfxMarkItem.H"

MarkSizeItem::MarkSizeItem(double ms):
  ms(ms), c("black"), shp(GfxMarkData::SolidCircle) {
}

MarkSizeItem::~MarkSizeItem() {
}

void MarkSizeItem::paintContents(QPainter *p) {
  GfxMarkItem::renderMark(QPointF(16, 16), c, ms, shp, p);
}

void MarkSizeItem::setColor(QColor c1) {
  c = c1;
  update();
}

void MarkSizeItem::setShape(GfxMarkData::Shape s1) {
  shp = s1;
  update();
}

void MarkSizeItem::setMarkSize(double s1) {
  ms = s1;
  update();
}

