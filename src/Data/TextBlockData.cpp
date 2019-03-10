// Data/TextBlockData.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// TextBlockData.C

#include "TextBlockData.h"
#include <QDebug>
#include "FootnoteData.h"
#include "ElnAssert.h"
#include "GfxNoteData.h"

static Data::Creator<TextBlockData> c("textblock");

TextBlockData::TextBlockData(Data *parent): BlockData(parent) {
  text_ = 0;
  setType("textblock");
  ind = 1;
  text_ = new TextData(this);
}

TextBlockData::~TextBlockData() {
}

void TextBlockData::setIndented(bool i) {
  if (i) {
    if ((ind&Indented) && !(ind&Dedented))
      return;
    ind |= Indented;
    ind &= ~Dedented;
  } else {
    if (!(ind&Indented))
      return;
    ind &= ~Indented;
  }
  markModified();
}

void TextBlockData::setDedented(bool i) {
  if (i) {
    if ((ind&Dedented) && !(ind&Indented))
      return;
    ind |= Dedented;
    ind &= ~Indented;
  } else {
    if (!(ind&Dedented))
      return;
    ind &= ~Dedented;
  }
  markModified();
}

bool TextBlockData::indented() const {
  return ind & Indented;
}

bool TextBlockData::dedented() const {
  return ind & Dedented;
}

void TextBlockData::setDisplayed(bool i) {
  if (i) {
    if (ind&Displayed)
      return;
    ind |= Displayed;
  } else {
    if (!(ind&Displayed))
      return;
    ind &= ~Displayed;
  }
  markModified();
}

bool TextBlockData::displayed() const {
  return ind & Displayed;
}

void TextBlockData::setIndentationStyle(int i) {
  if (ind==i)
    return;
  ind = i;
  markModified();
}

int TextBlockData::indentationStyle() const {
  return ind;
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
  ASSERT(text_);
}

bool TextBlockData::isEmpty() const {
  return BlockData::isEmpty() && (!text_ || text_->isEmpty());
}

TextBlockData *TextBlockData::split(int pos) {
  TextBlockData *block2 = deepCopy(this);
  TextData *text1 = this->text();
  TextData *text2 = block2->text();
  text1->setLineStarts(QVector<int>());
  text2->setLineStarts(QVector<int>());
  
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
    if (md->style()==MarkupData::FootnoteRef)
      tags1.insert(text1->text().mid(md->start(), md->end()-md->start()));
  foreach (FootnoteData *fd, this->children<FootnoteData>())
    if (!tags1.contains(fd->tag()))
      this->deleteChild(fd);

  QSet<QString> tags2;
  foreach (MarkupData *md, text2->children<MarkupData>()) 
    if (md->style()==MarkupData::FootnoteRef)
      tags2.insert(text2->text().mid(md->start(), md->end()-md->start()));
  foreach (FootnoteData *fd, block2->children<FootnoteData>())
    if (!tags2.contains(fd->tag()))
      block2->deleteChild(fd);

  foreach (GfxNoteData *nd, text2->children<GfxNoteData>())
    text2->deleteChild(nd);

  return block2;
}  

void TextBlockData::join(TextBlockData *block2) {
  TextData *text1 = text();
  TextData *text2 = block2->text();
  int pos = text1->text().size();

  // import text
  text1->setText(text1->text() + text2->text());
  text1->setLineStarts(QVector<int>());

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
      QString t0 = fnd0->text()->text();
      QString t1 = fnd->text()->text();
      if (t0!=t1)
	fnd0->text()->setText(t0 + " " + t1);
    } else {
      addChild(fnd);
    }
  }
  block2->deleteLater();
}
