// TextData.C

#include "TextData.H"

TextData::TextData(Data *parent):
  Data(parent) {
}

TextData::~TextData() {
}

QString TextData::text() const {
  return text_;
}

QList<MarkupData *> const &TextData::markups() const {
  return markups_;
}

void TextData::setText(QString const &t) {
  text_ = t;
  markModified();
}

MarkupData *TextData::addMarkup(int start, int end,
				     MarkupData::Style style) {
  MarkupData *md = new MarkupData(0);
  /* Initial parent-less construction avoids propagating markModified
     before initialization is complete. */
  md->setStart(start);
  md->setEnd(end);
  md->setStyle(style);
  markups_.append(md);
  md->setParent(this);
  markModified(InternalMod);
  return md;
}

void TextData::removeMarkup(MarkupData *md) {
  if (markups_.removeOne(md)) {
    delete md;
    markModified(InternalMod);
  }
}

void TextData::loadMore(QVariantMap const &src) {
  foreach (MarkupData *md, markups_)
    delete md;
  markups_.clear();
  
  QVariantList ml = src["markups"].toList();
  foreach (QVariant m, ml) {
    MarkupData *md = new MarkupData(this);
    md->load(m.toMap());
    markups_.append(md);
  }
}

void TextData::saveMore(QVariantMap &dst) const {
  QVariantList ml;
  foreach (MarkupData *md, markups_) {
    QVariantMap m = md->save();
    ml.append(m);
  }
  dst["markups"] = ml;
}
