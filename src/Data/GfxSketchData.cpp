// GfxSketchData.C

#include "GfxSketchData.H"

static Data::Creator<GfxSketchData> c("gfxsketch");

GfxSketchData::GfxSketchData(Data *parent): GfxPointsData(parent) {
  setType("gfxsketch");
  col = QColor("black");
  lw = 1;
}

GfxSketchData::~GfxSketchData() {
}

QColor GfxSketchData::color() const {
  return col;
}

double GfxSketchData::lineWidth() const {
  return lw;
}

void GfxSketchData::setColor(QColor c) {
  col = c;
  markModified();
}

void GfxSketchData::setLineWidth(double w) {
  lw = w;
  markModified();
}

    
