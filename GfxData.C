// GfxData.C

#include "GfxData.H"
#include "GfxBlockData.H"

GfxData::GfxData(Data *parent): Data(parent) {
  setType("gfx");
}

GfxData::~GfxData() {
}

double GfxData::x() const {
  return x_;
}
double GfxData::y() const {
  return y_;
}

void GfxData::setX(double x) {
  x_ = x;
  markModified();
}

void GfxData::setY(double y) {
  y_ = y;
  markModified();
}

QPointF GfxData::xy() const {
  return QPointF(x_, y_);
}

void GfxData::setXY(QPointF xy) {
  x_ = xy.x();
  y_ = xy.y();
  markModified();
}
