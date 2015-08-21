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
#include "Translate.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

TitleItem::TitleItem(TitleData *data, Item *parent):
  DefaultingTextItem(data->text(), parent), d(data) {
  setStyles();
}

void TitleItem::setStyles() {
  setFont(style().font("title-font"));
  setLineHeight(style().lineSpacing("title-font", 1));
  setDefaultTextColor(style().color("title-color"));
  setAllowParagraphs(false);
  setZValue(10); // stack before other stuff so that late notes go in front
  setDefaultText(Translate::_("Untitled"));
}

TitleItem::TitleItem(TitleData *data, int sheetno,
		     TextItemDoc *altdoc, Item *parent):
  DefaultingTextItem(data->text(), parent, true, altdoc), d(data) {
  setStyles();
  document()->buildLinePos();
  finalizeConstructor(sheetno);
}

TitleItem::~TitleItem() {
}

void TitleItem::deleteLater() {
  ASSERT(d);
  d = 0;
  TextItem::deleteLater();
}

void TitleItem::keyPressEvent(QKeyEvent *e) {
  if (e->key()==Qt::Key_Tab)
    emit futileMovementKey(Qt::Key_Enter, 0);
  else
    TextItem::keyPressEvent(e);
}
