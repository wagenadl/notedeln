// TextBlockData.C

#include "TextBlockData.H"
#include <QDebug>

static Data::Creator<TextBlockData> c("textblock");

TextBlockData::TextBlockData(Data *parent): BlockData(parent) {
  setType("textblock");
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
  BlockData::loadMore(src);
  // Our old text_ pointer is now invalidated: since it was part of
  // Data's children array, it has already been deleted.
  text_ = firstChild<TextData>();
  Q_ASSERT(text_);
}

bool TextBlockData::isEmpty() const {
  return BlockData::isEmpty() && text_->isEmpty();
}
