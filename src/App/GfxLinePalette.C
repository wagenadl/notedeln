// GfxLinePalette.C

#include "GfxLinePalette.H"
#include <math.h>
#include "GfxMarkItem.H"
#include <QPainter>
#include <QBrush>
#include <QPen>

static char const *colors[] = {
  "#000000",
  "#787878",
  "#C7C7C7",
  "#FFFFFF",
  "#EE2200",
  "#EE5F00",
  "#FFD400",
  "#005F00",
  "#00D400",
  "#0000CF",
  "#4C84FF",
  "#C400FF",
};

GfxLinePalette::GfxLinePalette(QGraphicsItem *parent): GfxPalette(parent) {
  setGridSize(6, sizeof(colors));
}

GfxLinePalette::~GfxLinePalette() {
}

QColor GfxLinePalette::colorAt(int, int col) {
  if (col<0 || col>=int(sizeof(colors)))
    return QColor();
  else
    return QColor(colors[col]);
}

double GfxLinePalette::sizeAt(int row, int) {
  return pow(2, (row-2)/2.0);
}
  
void GfxLinePalette::paintOne(int row, int col, QPainter *p) {
  QPointF p0((col+.5) * cellsize, (row+.5) * cellsize);
  p->setPen(QPen(colorAt(row, col), sizeAt(row, col)));
  p->setBrush(Qt::NoBrush);
  p->drawLine(-.4*cellsize, .9*cellsize,
	      .4*cellsize, -.9*cellsize);
}
