// GfxData.C

#include "GfxData.H"
#include "GfxBlockData.H"

QMap<QString, GfxData *(*)()> &GfxData::creators() {
  static QMap<QString, GfxData *(*)()> g;
  return g;
}

GfxData *GfxData::create(QString t) {
  if (creators().contains(t))
    return creators()[t]();
  else
    return 0;
}

GfxData::GfxData(GfxBlockData *parent): Data(parent) {
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

