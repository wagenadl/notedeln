// Data/TextData.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// TextData.C

#include "TextData.h"
#include <QDebug>
#include <QRegularExpression>

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

void TextData::setText(QString const &t, bool hushhush) {
  bool wasempty = text_=="";
  if (text_==t)
    return;
  text_ = t;
  wordset_.clear();
  if (!hushhush) {
    if (wasempty) {
      QDateTime t = QDateTime::currentDateTime();
      setCreated(t);
      if (parent() && parent()->allChildren().size()==1) {
        parent()->setCreated(t);
        parent()->markModified();
      }
    }
    markModified();
  }
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

MarkupData *TextData::markupAt(int pos, MarkupData::Style typ) const {
  return markupAt(pos, pos, typ);
}

MarkupData *TextData::markupAt(int start, int end,
			       MarkupData::Style typ) const {
  foreach (MarkupData *md, children<MarkupData>())
    if (md->style()==typ && md->end()>=start && md->start()<=end)
      return md;
  return 0;
}

MarkupData *TextData::markupAt(int start, int end) const {
  foreach (MarkupData *md, children<MarkupData>())
    if (md->end()>=start && md->start()<=end)
      return md;
  return 0;
}

MarkupData *TextData::markupEdgeIn(int start, int end) const {
  foreach (MarkupData *md, children<MarkupData>())
    if ((md->start() > start && md->start() < end)
	|| (md->end() > start && md->end() < end))
      return md;
  return 0;
}
  

MarkupData *TextData::mergeMarkup(int start, int end, MarkupData::Style style,
				  bool *new_return) {
  MarkupData *md = new MarkupData(start, end, style);
  return mergeMarkup(md, new_return);
}

MarkupData *TextData::mergeMarkup(MarkupData *md, bool *new_return) {
  MarkupData *md0 = markupAt(md->start(), md->end(), md->style());
  if (md0 && mergeable(md, md0)) {
    int s0 = md0->end();
    MarkupData *md1 = markupAt(s0 + 1, md->end(), md->style());
    while (md1) { // get rid of other overlaps
      s0 = md1->end();
      if (s0 > md0->end())
        md0->setEnd(s0);
      deleteMarkup(md1);
      md1 = markupAt(s0 + 1, md->end(), md->style());
    }
    if (md->start()<md0->start())
      md0->setStart(md->start());
    if (md->end()>md0->end())
      md0->setEnd(md->end());
    md->deleteLater();
    if (new_return)
      *new_return = false;
    return md0;
  } else {
    addMarkup(md);
    if (new_return)
      *new_return = true;
    return md;
  }
}

int TextData::offsetOfFootnoteTag(QString s) const {
  int l = s.length();
  foreach (MarkupData *md, markups()) 
    if (md->style()==MarkupData::FootnoteRef
        && md->end()-md->start()==l
        && text_.mid(md->start(), l)==s)
      return md->start();
  return -1;
}

void TextData::loadMore(QVariantMap const &src) {
  Data::loadMore(src);
  linestarts.clear();
  if (src.contains("lines"))
    for (auto &v: src["lines"].toList())
      linestarts.append(v.toInt());  
}

void TextData::saveMore(QVariantMap &dst) const {
  Data::saveMore(dst);
  if (!linestarts.isEmpty()) {
    QVariantList xl;
    foreach (int i, linestarts)
      xl.append(QVariant(i));
    dst["lines"] = QVariant(xl);
  }
}

QVector<int> const &TextData::lineStarts() const {
  return linestarts;
}

void TextData::setLineStarts(QVector<int> const &s) {
  linestarts = s;
  markModified(InternalMod);
}

QList<int> TextData::paragraphStarts() const {
  QList<int> res;
  int off = 0;
  while (off>=0) {
    res << off;
    off = text_.indexOf("\n", off+1);
    if (off>=0)
      off++;
  }
  return res;
}

QSet<QString> TextData::wordSet() const {
  if (wordset_.isEmpty() && !text_.isEmpty()) {
    for (QString w: text_.split(QRegularExpression("\\W+")))
      if (!w.isEmpty())
        wordset_ << w.toLower();
  }
  return wordset_ | Data::wordSet();
}
