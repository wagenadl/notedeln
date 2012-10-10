// TextBlockData.C

#include "TextBlockData.H"
#include <QDebug>

TextBlockData::TextBlockData(class PageData *parent):
  BlockData(parent) {
  text_ = new TextData(this);
}

TextBlockData::~TextBlockData() {
}

TextData const *TextBlockData::text() const {
  return text_;
}

TextData *TextBlockData::text() {
  return text_;
}

void TextBlockData::loadMore(QVariantMap const &src) {
  text_->load(src["text"].toMap());
}

void TextBlockData::saveMore(QVariantMap &dst) const {
  dst["text"] = text_->save();
}
