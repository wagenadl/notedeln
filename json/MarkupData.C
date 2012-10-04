// MarkupData.C

#include "MarkupData.H"
#include "TextBlockData.H"

MarkupData::MarkupData(TextBlockData *parent):
  Data(parent) {
}

MarkupData::~MarkupData() {
}

int MarkupData::start() const {
  return start_;
}

int MarkupData::end() const {
  return end_;
}

MarkupData::Style MarkupData::style() const {
  return style_;
}

void MarkupData::setStart(int i) {
  start_ = i;
  markModified(NonPropMod);
}

void MarkupData::setEnd(int i) {
  end_ = i;
  markModified(NonPropMod);
}

void MarkupData::setStyle(Style s) {
  style_ = s;
  markModified(NonPropMod);
}

