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
  setGridSize(sizeof(colors), 6);
}

GfxLinePalette::~GfxLinePalette() {
}

QColor GfxLinePalette::colorAt(int c, int) {
  if (c<0 || c>=int(sizeof(colors)))
    return QColor();
  else
    return QColor(colors[c]);
}

double GfxLinePalette::sizeAt(int, int s) {
  return 0.5 * pow(2, s/2.0);
}
  
void GfxLinePalette::paintOne(int row, int col, QPainter *p) {
  QPointF p0((col+.5) * cellsize, (row+.5) * cellsize);
  p->setPen(QPen(colorAt(row, col), sizeAt(row, col)));
  p->setBrush(Qt::NoBrush);
  p->drawLine(-.4*cellsize, .9*cellsize,
	      .4*cellsize, -.9*cellsize);
}
