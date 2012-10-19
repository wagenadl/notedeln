// GfxNoteData.C

#include "GfxNoteData.H"
#include <math.h>

static Data::Creator<GfxNoteData> c("gfxnote");

GfxNoteData::GfxNoteData(Data *parent): GfxData(parent) {
  text_ = new TextData(this);
  addChild(text_); // we store the text as our first child
  setType("gfxnote");
}

GfxNoteData::~GfxNoteData() {
}

GfxNoteData::HAlign GfxNoteData::hAlign() const {
  return hAlign_;
}

GfxNoteData::VAlign GfxNoteData::vAlign() const {
  return vAlign_;
}

double GfxNoteData::dx() const {
  return dx_;
}

double GfxNoteData::dy() const {
  return dx_;
}

double GfxNoteData::textWidth() const {
  return textWidth_;
}

void GfxNoteData::setHAlign(GfxNoteData::HAlign ha) {
  hAlign_ = ha;
  markModified();
}

void GfxNoteData::setVAlign(GfxNoteData::VAlign va) {
  vAlign_ = va;
  markModified();
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

QPointF GfxNoteData::endPoint() const {
  return pos() + delta();
}

QPointF GfxNoteData::delta() const {
  return QPointF(dx(), dy());
}

double GfxNoteData::lineLength() const {
  return sqrt(dx()*dx() + dy()*dy());
}

void GfxNoteData::setEndPoint(QPointF p, bool realign) {
  dx_ = p.x() - x();
  dy_ = p.y() - y();
  if (realign) {
    hAlign_ = dx_<0 ? Right : Left;
    vAlign_ = Middle;
  }
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
  QList<TextData*> l(children<TextData>());
  Q_ASSERT(!l.isEmpty());
  text_ = l[0];
  Q_ASSERT(text_);
}
