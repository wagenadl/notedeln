// FootnoteData.C

#include "FootnoteData.H"

static Data::Creator<FootnoteData> c("footnote");

FootnoteData::FootnoteData(Data *parent): TextData(parent) {
  setType("footnote");
}

FootnoteData::~FootnoteData() {
}

QString FootnoteData::tag() const {
  return tag_;
}

void FootnoteData::setTag(QString t) {
  tag_ = t;
  markModified();
}
