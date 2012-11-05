// GfxDoodleData.C

#include "GfxDoodleData.H"

static Data::Creator<GfxDoodleData> c("gfxdoodle");

GfxDoodleData::GfxDoodleData(Data *parent): GfxPointsData(parent) {
  col = QColor("black");
  lw = 1;
}

GfxDoodleData::~GfxDoodleData() {
}

QColor GfxDoodleData::color() const {
  return col;
}

double GfxDoodleData::lineWidth() const {
  return lw;
}

void GfxDoodleData::setColor(QColor c) {
  col = c;
  markModified();
}

void GfxDoodleData::setLineWidth(double w) {
  lw = w;
  markModified();
}

    
