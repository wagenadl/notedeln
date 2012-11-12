// GfxPalette.C

#include "GfxPalette.H"
#include <QEventLoop>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>
#include <math.h>

QColor GfxPalette::c("black");
double GfxPalette::wid = 1;
double GfxPalette::siz = 5;
GfxMarkData::Shape GfxPalette::shp(GfxMarkData::SolidCircle);

GfxPalette::GfxPalette(QGraphicsItem *parent): QGraphicsObject(parent) {
  rows = 1;
  cols = 1;
  cellsize = 72.0/2;
  hide();
}
  
GfxPalette::~GfxPalette() {
}

QColor GfxPalette::color() {
  return c;
}

double GfxPalette::lineWidth() {
  return wid;
}

double GfxPalette::markSize() {
  return siz;
}

GfxMarkData::Shape GfxPalette::markShape() {
  return shp;
}

void GfxPalette::setColor(QColor c_) {
  c = c_;
}

void GfxPalette::setLineWidth(double s) {
  wid = s;
}

void GfxPalette::setMarkSize(double s) {
  siz = s;
}

void GfxPalette::setMarkShape(GfxMarkData::Shape s) {
  shp = s;
}

QRectF GfxPalette::boundingRect() const {
  return QRectF(0, 0, cellsize*cols, cellsize*rows);
}

void GfxPalette::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  QPointF p = e->pos();
  int nrow = floor(p.y()/cellsize);
  int ncol = floor(p.x()/cellsize);
  if (nrow!=row || ncol!=col) {
    row = nrow;
    col = ncol;
    update();
  }
}

void GfxPalette::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
  emit release();
}

static double sq(double x) {
  return x*x;
}

bool GfxPalette::findMe() {
  double ds = 1e9;
  for (int r=0; r<rows; r++) {
    for (int c=0; c<cols; c++) {
      if (colorAt(r, c) == color()
	  && shapeAt(r, c) == markShape()) {
	double ds1 = sq(sizeAt(r, c)-markSize()) + sq(widthAt(r,c)-lineWidth());
	if (ds1 < ds) {
	  row = r;
	  col = c;
	  ds = ds1;
	}
      }
    }
  }
  return ds<100;
}
      
bool GfxPalette::letUserChoose(QGraphicsScene *scene, QPointF p0) {
  QObject *origParent = parent();
  QEventLoop el;
  connect(this, SIGNAL(release()), &el, SLOT(quit()));

  scene->addItem(this);
  if (!findMe()) {
    col = cols/2;
    row = rows/2;
  }
  setPos(p0-QPointF((col+.5)*cellsize, (row+.5)*cellsize));
  setZValue(1000);
  show();
  grabMouse();
  el.exec();
  ungrabMouse();
  hide();
  scene->removeItem(this);
  setParent(origParent);
  
  if (row>=0 && row<rows && col>=0 && col<cols) {
    // selection made
    c = colorAt(row, col);
    wid = widthAt(row, col);
    siz = sizeAt(row, col);
    shp = shapeAt(row, col);
    return true;
  } else {
    return false;
  }
}

void GfxPalette::paint(QPainter *p,
		       const QStyleOptionGraphicsItem *,
		       QWidget *) {
  p->setBrush(QBrush("#ffffff"));
  p->setPen("#000000");
  p->drawRect(boundingRect());
  for (int r=0; r<rows; r++) {
    for (int c=0; c<cols; c++) {
      if (r==row && c==col) {
	// p->setPen("#000000");
	p->setPen(Qt::NoPen);
	p->setBrush(QBrush("#dddddd"));
	p->drawRect(QRectF(cellsize*c, cellsize*r, cellsize, cellsize));
      } else if (colorAt(r, c) == QColor("#ffffff")) {
	p->setPen(Qt::NoPen);
	p->setBrush(QBrush("#aaaaaa"));
	p->drawRect(QRectF(cellsize*c, cellsize*r, cellsize, cellsize));
      }
      paintOne(r, c, p);
    }
  }
}

void GfxPalette::setGridSize(int r, int c) {
  rows = r;
  cols = c;
}

void GfxPalette::setCellSize(double s) {
  cellsize = s;
}

QColor GfxPalette::colorAt(int, int) {
  return c;
}

double GfxPalette::widthAt(int, int) {
  return wid;
}

double GfxPalette::sizeAt(int, int) {
  return siz;
}

GfxMarkData::Shape GfxPalette::shapeAt(int, int) {
  return shp;
}
