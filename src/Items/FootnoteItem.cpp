// Items/FootnoteItem.cpp - This file is part of eln

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

// FootnoteItem.C

#include "FootnoteItem.H"
#include "FootnoteData.H"
#include "TextItem.H"
#include "AutoNote.H"
#include "Assert.H"

#include <QDebug>

FootnoteItem::FootnoteItem(FootnoteData *data, Item *parent):
  TextBlockItem(data, parent) {
  ASSERT(data->book());
  tag_ = new QGraphicsTextItem(this);

  tag_->setFont(style().font("footnote-tag-font"));
  tag_->setDefaultTextColor(style().color("footnote-tag-color"));

  text()->setFont(style().font("footnote-def-font"));
  text()->setDefaultTextColor(style().color("footnote-def-color"));

  updateTag();
  text()->setAllowParagraphs(false);

  QTextCursor tc(text()->document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(100, QTextBlockFormat::ProportionalHeight);
  fmt.setTextIndent(0.0);
  fmt.setTopMargin(0.0);
  fmt.setBottomMargin(0.0);
  tc.setBlockFormat(fmt);
  text()->setTextWidth(text()->textWidth()); // crazy way to fix HoverRegions
  connect(text(), SIGNAL(abandoned()), this, SLOT(abandon()));
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

