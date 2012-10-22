// GfxBlockData.C

#include "GfxBlockData.H"
#include "GfxData.H"
#include <QDebug>

static Data::Creator<GfxBlockData> c("gfxblock");

GfxBlockData::GfxBlockData(Data *parent):
  BlockData(parent) {
  setType("gfxblock");
  xref_ = yref_ = 0;
}

GfxBlockData::~GfxBlockData() {
}

QList<class GfxData *> GfxBlockData::gfx() const{
  return children<GfxData>();
}

double GfxBlockData::xref() const {
  return xref_;
}

double GfxBlockData::yref() const {
  return yref_;
}

QPointF GfxBlockData::ref() const {
  return QPointF(xref_, yref_);
}

void GfxBlockData::setXref(double x) {
  xref_ = x;
  markModified();
}


void GfxBlockData::setYref(double y) {
  yref_ = y;
  markModified();
}


void GfxBlockData::setRef(QPointF xy) {
  xref_ = xy.x();
  yref_ = xy.y();
  markModified();
}
