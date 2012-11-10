// GfxMarkData.C

#include "GfxMarkData.H"

static Data::Creator<GfxMarkData> c("gfxmark");

GfxMarkData::GfxMarkData(Data *parent): GfxData(parent) {
  setType("gfxmark");
  col = QColor("black");
  siz = 5;
  shp = SolidCircle;
}

GfxMarkData::~GfxMarkData() {
}

QColor GfxMarkData::color() const {
  return col;
}
    
double GfxMarkData::size() const {
  return siz;
}

GfxMarkData::Shape GfxMarkData::shape() const {
  return shp;
}

void GfxMarkData::setColor(QColor c) {
  col = c;
  markModified();
}

void GfxMarkData::setSize(double s) {
  siz = s;
  markModified();
}

void GfxMarkData::setShape(Shape s) {
  shp = s;
  markModified();
}

