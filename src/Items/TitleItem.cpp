// Items/TitleItem.cpp - This file is part of eln

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

// TitleItem.C

#include "TitleItem.h"
#include "Style.h"
#include <QDebug>
#include <QTextDocument>

TitleItem::TitleItem(TitleData *data, Item *parent):
  TextItem(data->text(), parent), d(data) {

  setFont(style().font("title-font"));
  setLineHeight(style().lineSpacing("title-font", 1));
  setDefaultTextColor(style().color("title-color"));
  setAllowParagraphs(false);
  //  document()->recalculatebuildLinePos();
  setZValue(10); // stack before other stuff so that late notes go in front
}

TitleItem::TitleItem(TitleData *data, int sheetno,
		     TextItemDoc *altdoc, Item *parent):
  TextItem(data->text(), parent, true, altdoc), d(data) {

  setFont(style().font("title-font"));
  setLineHeight(style().lineSpacing("title-font", 1));
  setDefaultTextColor(style().color("title-color"));
  setAllowParagraphs(false);
  document()->buildLinePos();
  setZValue(10); // stack before other stuff so that late notes go in front
  finalizeConstructor(sheetno);
}


TitleItem::~TitleItem() {
}

void TitleItem::deleteLater() {
  ASSERT(d);
  d = 0;
  TextItem::deleteLater();
}

void TitleItem::focusInEvent(QFocusEvent *e) {
  if (text->text()==TitleData::defaultTitle()) {
    // select the title
    TextCursor c = textCursor();
    c.setPosition(0);
    c.movePosition(TextCursor::End, TextCursor::KeepAnchor);
    setTextCursor(c);
  }
  TextItem::focusInEvent(e);
}

