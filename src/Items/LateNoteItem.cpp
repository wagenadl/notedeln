// Items/LateNoteItem.cpp - This file is part of eln

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

// LateNoteItem.C

#include "LateNoteItem.h"
#include "LateNoteData.h"
#include "TextItem.h"
#include "EntryData.h"
#include "Assert.h"

#include <QPen>
#include <QBrush>
#include <QDebug>

static Item::Creator<LateNoteData, LateNoteItem> c("latenote");

LateNoteItem::LateNoteItem(LateNoteData *data, Item *parent):
  GfxNoteItem(data, parent) {
  if (line)
    line->setPen(QPen(QBrush(QColor(style().string("latenote-line-color"))),
		      style().real("latenote-line-width")));
  text->setDefaultTextColor(QColor(style().string("latenote-text-color")));
  text->setFont(style().font("latenote-font"));
  prepDateItem();
  if (data->isRecent()) {
    makeWritable();
  }
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
}

LateNoteItem::~LateNoteItem() {
}

void LateNoteItem::prepDateItem() {
  dateItem = new QGraphicsTextItem(this);
  dateItem->setFont(style().font("latenote-font"));
  dateItem->setDefaultTextColor(QColor(style().string("latenote-text-color")));
  QDateTime myDate = data()->created();
  QString lbl = myDate.toString(style().string("date-format"));
  dateItem->setPlainText(lbl);
  setDateItemPosition();
}

QVariant LateNoteItem::itemChange(GraphicsItemChange change,
				  QVariant const &value) {
  switch (change) {
  case ItemScenePositionHasChanged:
    // this actually happens when the page title gets positioned
    setDateItemPosition();
    break;
  default:
    break;
  }
  return GfxNoteItem::itemChange(change, value);
}

void LateNoteItem::setDateItemPosition() {
  QPointF sp = scenePos();
  QPointF tp = text->pos();
  QRectF br = dateItem->sceneBoundingRect();
  double ml = style().real("margin-left");
  if (mapToScene(tp).x()>=ml) 
    dateItem->setPos(QPointF(ml - br.width() - 2 - sp.x(), tp.y()));
  else
    dateItem->setPos(tp.x(), tp.y() - style().real("latenote-yshift"));
}

void LateNoteItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
  GfxNoteItem::mouseMoveEvent(e);
  setDateItemPosition();
}

void LateNoteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
  GfxNoteItem::mouseReleaseEvent(e);
  setDateItemPosition();
}

void LateNoteItem::setScale(qreal f) {
  GfxNoteItem::setScale(f);
  setDateItemPosition();
}

bool LateNoteItem::excludeFromNet() const {
  return true;
}
