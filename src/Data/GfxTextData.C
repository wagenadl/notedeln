// GfxTextData.C

#include "GfxTextData.H"

static Data::Creator<GfxTextData> c("gfxtext");

GfxTextData::GfxTextData(Data *parent): GfxData(parent) {
  setType("gfxtext");
}

GfxTextData::~GfxTextData() {
}

GfxTextData::HAlign GfxTextData::hAlign() const {
  return hAlign_;
}

GfxTextData::VAlign GfxTextData::vAlign() const {
  return vAlign_;
}

void GfxTextData::setHAlign(GfxTextData::HAlign ha) {
  hAlign_ = ha;
  markModified();
}

void GfxTextData::setVAlign(GfxTextData::VAlign va) {
  vAlign_ = va;
  markModified();
}


QString GfxTextData::text() const {
  return text_;
}

QList<MarkupData *> GfxTextData::markups() const {
  return children<MarkupData>();
}

void GfxTextData::setText(QString const &t) {
  text_ = t;
  markModified();
}

MarkupData *GfxTextData::addMarkup(int start, int end,
				     MarkupData::Style style) {
  MarkupData *md = new MarkupData(start, end, style, this);
  addChild(md, InternalMod);
  return md;
}

void GfxTextData::deleteMarkup(MarkupData *md) {
  deleteChild(md, InternalMod);
}

