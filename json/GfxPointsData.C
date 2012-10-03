// GfxPointsData.C

#include "GfxPointsData.H"

static GfxData::Creator<GfxPointsData> c("points");

GfxPointsData::GfxPointsData(GfxBlockData *parent):
  GfxData(parent) {
}

GfxPointsData::~GfxPointsData() {
}

QList<double> const &GfxPointsData::xx() const {
  return xx_;
}

QList<double> const &GfxPointsData::yy() const {
  return yy_;
}

void GfxPointsData::addPoint(QPointF p) {
  xx_.append(p.x());
  yy_.append(p.y());
  markModified();
}

void GfxPointsData::addPoint(double x, double y) {
  xx_.append(x);
  yy_.append(y);
  markModified();
}

void GfxPointsData::removePoint(int i) {
  xx_.erase(i);
  yy_.erase(i);
  markModified();
}

void GfxPointsData::setXX(QList<double> const &x) {
  xx_ = x;
  // Since this should only be called from load(), we don't need to
  // call markModified()
}

void GfxPointsData::setYY(QList<double> const &y) {
  yy_ = y;
}

