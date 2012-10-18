// TextBlockData.C

#include "TextBlockData.H"
#include <QDebug>

static Data::Creator<TextBlockData> c("textblock");

TextBlockData::TextBlockData(Data *parent):
  BlockData(parent) {
  text_ = new TextData(this);
  addChild(text_); // we store the text as our first child
  setType("textblock");
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
  QList<TextData*> l(children<TextData>());
  Q_ASSERT(!l.isEmpty());
  text_ = l[0];
  Q_ASSERT(text_);
}
