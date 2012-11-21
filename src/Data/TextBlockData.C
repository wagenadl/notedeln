// TextBlockData.C

#include "TextBlockData.H"
#include <QDebug>
#include "FootnoteData.H"

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

TextBlockData *TextBlockData::split(int pos) {
  TextBlockData *block2 = deepCopy(this);
  TextData *text1 = this->text();
  TextData *text2 = block2->text();
  
  text1->setText(text1->text().left(pos));
  foreach (MarkupData *md, text1->children<MarkupData>()) {
    if (md->start()>=pos)
      text1->deleteChild(md);
    else if (md->end()>=pos)
      md->setEnd(pos);
  }
  text2->setText(text2->text().mid(pos));
  foreach (MarkupData *md, text2->children<MarkupData>()) {
    if (md->end()<=pos) {
      text2->deleteChild(md);
    } else {
      md->setStart(md->start()<=pos ? 0 : md->start()-pos);
      md->setEnd(md->end()-pos);
    }
  }
  QSet<QString> tags1;
  foreach (MarkupData *md, text1->children<MarkupData>()) 
    if (md->style()==MarkupData::CustomRef)
      tags1.insert(text1->text().mid(md->start(), md->end()-md->start()));
  foreach (FootnoteData *fd, this->children<FootnoteData>())
    if (!tags1.contains(fd->tag()))
      this->deleteChild(fd);
  QSet<QString> tags2;
  foreach (MarkupData *md, text2->children<MarkupData>()) 
    if (md->style()==MarkupData::CustomRef)
      tags2.insert(text2->text().mid(md->start(), md->end()-md->start()));
  foreach (FootnoteData *fd, block2->children<FootnoteData>())
    if (!tags2.contains(fd->tag()))
      block2->deleteChild(fd);

  return block2;
}  

void TextBlockData::join(TextBlockData *block2) {
  TextData *text1 = text();
  TextData *text2 = block2->text();
  int pos = text1->text().size();

  // import text
  text1->setText(text1->text() + text2->text());

  // import markups and merge as needed
  foreach (MarkupData *md, text2->children<MarkupData>()) {
    text2->takeChild(md);
    md->setStart(md->start()+pos);
    md->setEnd(md->end()+pos);
    if (md->start()==pos) {
      // consider merging it with another
      foreach (MarkupData *md0, text1->children<MarkupData>()) {
	if (mergeable(md0, md)) {
	  md0->merge(md);
	  delete md;
	  md = 0;
	  break;
	}
      }
    }
    if (md)
      text1->addChild(md);
  }

  QMap<QString, FootnoteData *> fnd1;
  foreach (FootnoteData *fnd, children<FootnoteData>())
    fnd1[fnd->tag()] = fnd;
  foreach (FootnoteData *fnd, block2->children<FootnoteData>()) {
    block2->takeChild(fnd);
    QString tag = fnd->tag();
    if (fnd1.contains(tag)) {
      FootnoteData *fnd0 = fnd1[tag];
      fnd0->text()->setText(fnd0->text()->text() + " " +
			    fnd->text()->text());
    } else {
      addChild(fnd);
    }
  }
  block2->deleteLater();
}
