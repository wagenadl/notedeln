// TextData.C

#include "TextData.H"

static Data::Creator<TextData> c("text");

TextData::TextData(Data *parent):
  Data(parent) {
  setType("text");
}

TextData::~TextData() {
}

bool TextData::isEmpty() const {
  return text_.isEmpty() && markups().isEmpty();
}

QString TextData::text() const {
  return text_;
}

QList<MarkupData *> TextData::markups() const {
  return children<MarkupData>();
}

void TextData::setText(QString const &t) {
  text_ = t;
  markModified();
}

MarkupData *TextData::addMarkup(int start, int end,
				     MarkupData::Style style) {
  MarkupData *md = new MarkupData(start, end, style);
  addChild(md, InternalMod);
  return md;
}

void TextData::addMarkup(MarkupData *md) {
  addChild(md, InternalMod);
} 

void TextData::deleteMarkup(MarkupData *md) {
  deleteChild(md, InternalMod);
}

// void TextData::append(TextData const *other) {
//   int len = text_.size();
// 
//   QMap<MarkData::Style, MarkupData *> endMarks;
//   foreach (MarkupData *md, markups())
//     if (md->end()==len)
//       lastMarks[md->style()] = md;
//   
//   text_ += other->text_;
//   for (MarkupData const *md, other->markups()) {
//     if (md->start()==0 && endMarks.contains(md->style())) {
//       // importing a markup that abuts a markup of same style in our list
//       MarkupData shf(md->start()+len, md->end()+len, md->style());
//       endMarks[shf.style()]->merge(shf);
//     } else {
//       addMarkup(md->start()+len, md->end()+len, md->style());
//     }
//   }
// }
// 
// TextData *TextData::split(int pos) {
//   TextData *res = new TextData();
//   res->setText(text_.mid(pos));
//   text_ = text_.left(pos);
// 
//   foreach (MarkupData *md, markups()) {
//     if (md->end()>pos) {
//       int start = md->start()-pos;
//       if (start<0)
// 	start = 0;
//       res->addMarkup(start, md->end()-pos, md->style());
//       md->setEnd(pos);
//     }
//     if (md->start()>=pos)
//       deleteMarkup(md);
//   }
//   res->setParent(parent());
//   return res;
// }
// 
