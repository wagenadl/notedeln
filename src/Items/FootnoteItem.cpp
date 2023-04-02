// Items/FootnoteItem.cpp - This file is part of NotedELN

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

// FootnoteItem.C

#include "FootnoteItem.h"
#include "FootnoteData.h"
#include "TextItem.h"
#include "AutoNote.h"
#include "ElnAssert.h"

#include <QDebug>

FootnoteItem::FootnoteItem(FootnoteData *data, Item *parent):
  TextBlockItem(data, parent) {
  ASSERT(data->book());
  tag_ = new QGraphicsTextItem(this);

  tag_->setFont(style().font("footnote-tag-font"));
  tag_->setDefaultTextColor(style().color("footnote-tag-color"));

  text()->setFont(style().font("footnote-def-font"));
  text()->setLineHeight(style().lineSpacing("footnote-def-font", 1.15));
  text()->setDefaultTextColor(style().color("footnote-def-color"));

  updateTag();
  text()->setAllowParagraphs(false);
  text()->document()->setIndent(0);
  text()->document()->relayout();

  connect(text(), &TextItem::abandoned, this, &FootnoteItem::abandon);
}

FootnoteItem::~FootnoteItem() {
}

bool FootnoteItem::setAutoContents() {
  return AutoNote::autoNote(data()->tag(), text(), style());
}

QGraphicsTextItem *FootnoteItem::tag() {
  return tag_;
}

void FootnoteItem::setTagText(QString t) {
  data()->setTag(t);
  updateTag();
  sizeToFit();
}

QString FootnoteItem::tagText() const {
  return data()->tag();
}

void FootnoteItem::updateTag() {
  tag_->setPlainText(data()->tag() + ":");
  double textwidth = style().real("page-width")
    - style().real("margin-left")
    - style().real("margin-right");
  double tagwidth = tag_->boundingRect().width();
  text()->setPos(tagwidth, 0);
  text()->setTextWidth(textwidth - tagwidth);
}

void FootnoteItem::abandon() {
  // Until we figure out how to unmark references, let's just not
  // delete abandoned notes.
}

