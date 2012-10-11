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

