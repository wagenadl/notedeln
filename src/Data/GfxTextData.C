// GfxTextData.C

#include "GfxTextData.H"

static Data::Creator<GfxTextData> c("gfxtext");

GfxTextData::GfxTextData(Data *parent): GfxData(parent) {
  setType("gfxtext");
  text_ = new TextData(this);
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

TextData *GfxTextData::text() const {
  return text_;
}

void GfxTextData::loadMore(QVariantMap const &src) {
  text_->loadMore(src["text"].toMap());
}

void GfxTextData::saveMore(QVariantMap &dst) const {
  QVariantMap m;
  text_->saveMore(m);
  dst["text"] = m;
}
