// GfxMarkPalette.C

#include "GfxMarkPalette.H"
#include <math.h>
#include "GfxMarkItem.H"

GfxMarkPalette::GfxMarkPalette(QGraphicsItem *parent): GfxPalette(parent) {
  setGridSize(8, 6);
}

GfxMarkPalette::~GfxMarkPalette() {
}

double GfxMarkPalette::sizeAt(int, int s) {
  return 72./32 * pow(2, s/2.0);
}

GfxMarkData::Shape GfxMarkPalette::shapeAt(int s, int) {
  if (s<0 || s>GfxMarkData::LAST)
    return GfxMarkData::SolidCircle; // bailout
  else
    return GfxMarkData::Shape(s);
}
  
void GfxMarkPalette::paintOne(int row, int col, QPainter *p) {
  QPointF p0((col+.5) * cellsize, (row+.5) * cellsize);
  GfxMarkItem::renderMark(p0,
			  colorAt(row, col),
			  sizeAt(row, col),
			  shapeAt(row, col),
			  p);
}
