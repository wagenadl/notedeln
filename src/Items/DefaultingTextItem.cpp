// Items/DefaultingTextItem.cpp - This file is part of NotedELN

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

QRectF DefaultingTextItem::boundingRect() const {
  QRectF rct = TextItem::boundingRect();
  double textwidth = style().real("page-width")
    - style().real("margin-left")
    - style().real("margin-right");
  if (rct.width() < textwidth)
    rct.setWidth(textwidth);
  return rct;
}

