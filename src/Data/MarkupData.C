// MarkupData.C

#include "MarkupData.H"
#include "TextData.H"
#include <QDebug>

static Data::Creator<MarkupData> c("markup");

MarkupData::MarkupData(Data *parent):
  Data(parent) {
  setType("markup");
}

MarkupData::MarkupData(int start, int end, MarkupData::Style style,
		       Data *parent):
  Data(parent), start_(start), end_(end), style_(style) {
  setType("markup");
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

bool MarkupData::operator<(MarkupData const &other) const {
  if (start_<other.start_)
    return true;
  else if (start_>other.start_)
    return false;

  if (end_<other.end_)
    return true;
  else if (end_>other.end_)
    return false;

  return style_<other.style_;
}

bool mergeable(MarkupData const *a, MarkupData const *b) {
  return a->style_ == b->style_
    && a->style_ != MarkupData::FootnoteRef
    && a->style_ != MarkupData::Link
    && a->start_ <= b->end_
    && a->end_ >= b->start_;
}

void MarkupData::merge(MarkupData const *other) {
  Q_ASSERT(style_ == other->style_);
  if (other->created()<created())
    setCreated(other->created());
  if (other->modified()>modified())
    setModified(other->modified());
  if (other->start_ < start_)
    start_ = other->start_;
  if (other->end_ > end_)
    end_ = other->end_;
  markModified(InternalMod);
}

bool MarkupData::cut(int pos, int len) {
  bool chg = false;
  if (start_>pos) {
    start_ -= len;
    if (start_<pos)
      start_ = pos;
    chg = true;
  }
  if (end_>pos) {
    end_ -= len;
    if (end_<pos)
      end_ = pos;
    chg = true;
  }
  return chg;
}  

bool MarkupData::insert(int pos, int len) {
  bool chg = false;
  if (end_>pos || (end_==pos && end_==start_)) {
    end_ += len;
    chg = true;
  }
  if (start_>=pos) {
    start_ += len;
    chg = true;
  }
  return chg;
}

bool MarkupData::update(int pos, int del, int ins) {
  bool chg = false;
  if (del>ins) 
    chg = cut(pos, del-ins);
  else if (ins>del) 
    chg = insert(pos, ins-del);
  if (chg)
    markModified(InternalMod);
  return chg;
} 

QString MarkupData::text() const {
  TextData const *td = dynamic_cast<TextData const *>(parent());
  if (td) 
    return td->text().mid(start_, end_-start_);
  else
    return "";
}

QString MarkupData::styleName(Style s) {
  switch (s) {
  case Normal:
    return "rm";
  case Italic:
    return "it";
  case Bold:
    return "bf";
  case Underline:
    return "ul";
  case Link:
    return "link";
  case FootnoteRef:
    return "fnref";
  case Superscript:
    return "sup";
  case Subscript:
    return "sub";
  case StrikeThrough:
    return "so";
  case Emphasize:
    return "emph";
  }
  return "?";
}
