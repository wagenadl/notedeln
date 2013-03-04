// GfxNoteData.C

#include "GfxNoteData.H"
#include "Assert.H"
#include <math.h>

static Data::Creator<GfxNoteData> c("gfxnote");

GfxNoteData::GfxNoteData(Data *parent): GfxData(parent) {
  setType("gfxnote");
  text_ = new TextData(this);
  sheet_ = 0;
}

GfxNoteData::~GfxNoteData() {
}

double GfxNoteData::dx() const {
  return dx_;
}

double GfxNoteData::dy() const {
  return dy_;
}

double GfxNoteData::textWidth() const {
  return textWidth_;
}

int GfxNoteData::sheet() const {
  return sheet_;
}

void GfxNoteData::setDx(double d) {
  dx_ = d;
  markModified();
}

void GfxNoteData::setDy(double d) {
  dy_ = d;
  markModified();
}

void GfxNoteData::setTextWidth(double d) {
  textWidth_ = d;
  markModified();
}

void GfxNoteData::setSheet(int s) {
  sheet_ = s;
  markModified();
}

QPointF GfxNoteData::delta() const {
  return QPointF(dx(), dy());
}

void GfxNoteData::setDelta(QPointF p) {
  dx_ = p.x();
  dy_ = p.y();
  markModified();
}

TextData const *GfxNoteData::text() const {
  return text_;
}

TextData *GfxNoteData::text() {
  return text_;
}

void GfxNoteData::loadMore(QVariantMap const &src) {
  GfxData::loadMore(src);
  text_ = firstChild<TextData>();
  ASSERT(text_);
}
