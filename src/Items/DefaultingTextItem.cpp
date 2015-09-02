// Items/DefaultingTextItem.cpp - This file is part of eln

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

// DefaultingTextItem.cpp

#include "DefaultingTextItem.h"

DefaultingTextItem::DefaultingTextItem(TextData *data, Item *parent,
				       bool noFinalize, TextItemDoc *altdoc):
  TextItem(data, parent, noFinalize, altdoc) {
  connect(document(), SIGNAL(contentsChanged(int, int, int)),
	  this, SIGNAL(textChanged()));
}

DefaultingTextItem::~DefaultingTextItem() {
}

void DefaultingTextItem::setDefaultText(QString s) {
  dflt_text = s;
  update();
}

void DefaultingTextItem::paint(QPainter *p, const QStyleOptionGraphicsItem *o,
			       QWidget *w) {
  if (text->text().isEmpty()) {
    QPointF xy = text->locate(0);
    MarkupStyles sty;
    sty.add(MarkupData::Italic);
    p->setFont(text->font(sty));
    p->setPen(style().color("default-item-color"));
    p->drawText(xy, dflt_text);
  }
  TextItem::paint(p, o, w);
}
