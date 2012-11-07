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

static double lineWidths[] = {
  0.5,
  1,
  1.5,
  2,
  3,
  5,
  8,
};

static int nColors = sizeof(colors)/sizeof(*colors);
static int nLineWidths = sizeof(lineWidths)/sizeof(*lineWidths);

GfxLinePalette::GfxLinePalette(QGraphicsItem *parent): GfxPalette(parent) {
  setGridSize(nColors, nLineWidths);
}

GfxLinePalette::~GfxLinePalette() {
}

QColor GfxLinePalette::colorAt(int c, int) {
  if (c<0 || c>=nColors)
    return QColor();
  else
    return QColor(colors[c]);
}

double GfxLinePalette::sizeAt(int, int s) {
  if (s<0 || s>=nLineWidths)
    return 1;
  else
    return lineWidths[s];
}
  
void GfxLinePalette::paintOne(int row, int col, QPainter *p) {
  QPointF p0((col+.5) * cellsize, (row+.5) * cellsize);
  QPen pen(colorAt(row, col));
  pen.setWidthF(sizeAt(row, col));
  p->setPen(pen);
  p->setBrush(Qt::NoBrush);
  p->drawLine(p0.x()-.18*cellsize, p0.y()+.32*cellsize,
	      p0.x()+.18*cellsize, p0.y()-.32*cellsize);
}
