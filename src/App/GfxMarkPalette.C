// GfxMarkPalette.C

#include "GfxMarkPalette.H"
#include <math.h>
#include "GfxMarkItem.H"

GfxMarkPalette::GfxMarkPalette(QGraphicsItem *parent): GfxPalette(parent) {
  setGridSize(6, 8);
}

GfxMarkPalette::~GfxMarkPalette() {
}

double GfxMarkPalette::sizeAt(int row, int) {
  return pow(2, row/2.0);
}

GfxMarkData::Shape GfxMarkPalette::shapeAt(int, int col) {
  if (col<0 || col>GfxMarkData::LAST)
    return GfxMarkData::SolidCircle; // bailout
  else
    return GfxMarkData::Shape(col);
}
  
void GfxMarkPalette::paintOne(int row, int col, QPainter *p) {
  QPointF p0((col+.5) * cellsize, (row+.5) * cellsize);
  GfxMarkItem::renderMark(p0,
			  colorAt(row, col),
			  sizeAt(row, col),
			  shapeAt(row, col),
			  p);
}
