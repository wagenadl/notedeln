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

#include "TitleItem.H"
#include "Style.H"
#include <QDebug>
#include <QTextDocument>

TitleItem::TitleItem(TitleData *data, Item *parent):
  TextItem(data->current(), parent), d(data) {

  setFont(style().font("title-font"));
  setDefaultTextColor(style().color("title-color"));
  setAllowParagraphs(false);
  setZValue(10); // stack before other stuff so that late notes go in front
}

TitleItem::TitleItem(TitleData *data, int sheetno,
		     QTextDocument *altdoc, Item *parent):
  TextItem(data->current(), parent, true), d(data) {

  setFont(style().font("title-font"));
  setDefaultTextColor(style().color("title-color"));
  setAllowParagraphs(false);
  setZValue(10); // stack before other stuff so that late notes go in front

  if (altdoc)
    text->setDocument(altdoc);
  
  finalizeConstructor(sheetno);
}


TitleItem::~TitleItem() {
}

void TitleItem::deleteLater() {
  ASSERT(d);
  d = 0;
  TextItem::deleteLater();
}

void TitleItem::docChange() {
  QString plainText = text->toPlainText();
  if (data()->current()->text() != plainText)
    data()->revise();

  TextItem::docChange();
}
 
bool TitleItem::focusIn(QFocusEvent *) {
  if (text->toPlainText()==TitleData::defaultTitle()) {
    QTextCursor c = textCursor(); //(document());
    c.setPosition(0);
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    setTextCursor(c);
  }
  return false;
}

