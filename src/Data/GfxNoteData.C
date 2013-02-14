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

QPointF GfxNoteData::endPoint() const {
  return pos() + delta();
}

QPointF GfxNoteData::delta() const {
  return QPointF(dx(), dy());
}

double GfxNoteData::lineLength() const {
  return sqrt(dx()*dx() + dy()*dy());
}

bool GfxNoteData::lineLengthIsZero() const {
  return dx()==0 && dy()==0;
}

void GfxNoteData::setEndPoint(QPointF p) {
  dx_ = p.x() - x();
  dy_ = p.y() - y();
  markModified();
}

void GfxNoteData::setDelta(QPointF p) {
  dx_ = p.x();
  dy_ = p.y();
  markModified();
}

void GfxNoteData::setLineLengthToZero() {
  dx_ = dy_ = 0;
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
