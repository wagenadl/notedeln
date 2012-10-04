// GfxTextData.C

#include "GfxTextData.H"


GfxTextData::GfxTextData(class GfxBlockData *parent): GfxData(parent) {
}

GfxTextData::~GfxTextData() {
}

GfxTextData::HAlign GfxTextData::hAlign() const {
  return hAlign_;
}

GfxTextData::VAlign GfxTextData::vAlign() const {
  return vAlign_;
}

QString GfxTextData::text() const {
  return text_;
}

void GfxTextData::setHAlign(GfxTextData::HAlign ha) {
  hAlign_ = ha;
  markModified();
}

void GfxTextData::setVAlign(GfxTextData::VAlign va) {
  vAlign_ = va;
  markModified();
}

void GfxTextData::setText(QString t) {
  text_ = t;
  markModified();
}

