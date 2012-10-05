// TextBlockData.C

#include "TextBlockData.H"
#include <QDebug>

TextBlockData::TextBlockData(class PageData *parent):
  BlockData(parent) {
}

TextBlockData::~TextBlockData() {
}

QString TextBlockData::text() const {
  return text_;
}

//QList<int> const &TextBlockData::lineBreaks() const {
//  return lineBreaks_;
//}

QList<MarkupData *> const &TextBlockData::markups() const {
  return markups_;
}

void TextBlockData::setText(QString const &t) {
  if (!markups_.isEmpty())
    qDebug() << "TextBlockData: Caution: Changing texts while markups exist";
  text_ = t;
  markModified();
}

//void TextBlockData::setLineBreaks(QList<int> const &lb) {
//  lineBreaks_ = lb;
//  markModified(InternalMod);
//}

MarkupData *TextBlockData::addMarkup(int start, int end,
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

void TextBlockData::removeMarkup(MarkupData *md) {
  if (markups_.removeOne(md)) {
    delete md;
    markModified(InternalMod);
  }
}

void TextBlockData::loadMore(QVariantMap const &src) {
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

void TextBlockData::saveMore(QVariantMap &dst) const {
  QVariantList ml;
  foreach (MarkupData *md, markups_) {
    QVariantMap m = md->save();
    ml.append(m);
  }
  dst["markups"] = ml;
}
