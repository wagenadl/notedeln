// Items/TOCItem.cpp - This file is part of eln

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

// TOCItem.C

#include "TOCItem.h"
#include "TOCEntry.h"
#include "TOCScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QDateTime>

TOCItem::TOCItem(TOCEntry *data, BaseScene *parent):
  Item(data, 0), scene_(parent) {
  makeItems();
  fillItems();
  connect(data, SIGNAL(mod()), this, SLOT(entryChanged()));
}

TOCItem::~TOCItem() {
}

void TOCItem::makeItems() {
  dateItem = new QGraphicsTextItem(this);
  titleItem = new QGraphicsTextItem(this);
  pgnoItem = new QGraphicsTextItem(this);

  QFont f = style().font("toc-font");
  dateItem->setFont(f);
  titleItem->setFont(f);
  pgnoItem->setFont(f);

  dateItem->setDefaultTextColor(style().color("toc-date-color"));
  titleItem->setDefaultTextColor(style().color("toc-title-color"));
  pgnoItem->setDefaultTextColor(style().color("toc-pgno-color"));

  titleItem->setPos(QPointF(style().real("margin-left"), 0));
  titleItem->setTextWidth(style().real("page-width")
			  - style().real("margin-left")
			  - style().real("margin-right"));
}

void TOCItem::fillItems() {
  dateItem->setPlainText(data()->created()
			 .toString(style().string("toc-date-format")));
  titleItem->setPlainText(data()->title());
  pgnoItem->setPlainText(QString::number(data()->startPage()));
  
  dateItem->setPos(QPointF(style().real("margin-left")
			   - dateItem->boundingRect().width(), 0));
  pgnoItem->setPos(QPointF(style().real("page-width")
			   - style().real("margin-right-over")
			   - pgnoItem->boundingRect().width(), 
			   titleItem->boundingRect().height()
			   - pgnoItem->boundingRect().height()));
}

QRectF TOCItem::boundingRect() const {
  return childrenBoundingRect(); //QRectF();
}

void TOCItem::entryChanged() {
  QRectF r = childrenBoundingRect();
  fillItems();
  if (r != childrenBoundingRect())
    emit vboxChanged();
}

void TOCItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  emit clicked(data()->startPage(), e->modifiers());
}

void TOCItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}
