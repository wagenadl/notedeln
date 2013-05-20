// Items/FootnoteGroupItem.cpp - This file is part of eln

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

// FootnoteGroup.C

#include "FootnoteGroupItem.H"
#include "BlockData.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"
#include "EntryScene.H"

#include <QDebug>

FootnoteGroupItem::FootnoteGroupItem(BlockData *data, EntryScene *parent):
  Item(data, 0) {
  parent->addItem(this);
  foreach (FootnoteData *fd, data->children<FootnoteData>()) {
    FootnoteItem *fni = new FootnoteItem(fd, this);
    if (fd->height()==0) 
      fni->sizeToFit();
    fni->resetPosition();
  }
}

FootnoteGroupItem::~FootnoteGroupItem() {
}

void FootnoteGroupItem::makeWritable() {
  foreach (Item *c, children<FootnoteItem>())
    c->makeWritable();
}

double FootnoteGroupItem::netHeight() const {
  double h = 0;
  foreach (FootnoteData *fd, data()->children<FootnoteData>()) 
    h += fd->height();
  return h;
}  

bool FootnoteGroupItem::resetPosition() {
  bool anyzeros = false;
  foreach (FootnoteItem *c, children<FootnoteItem>()) {
    if (c->data()->y0()==0)
      anyzeros = true;
    c->resetPosition();
  }
  return anyzeros;
}  


QRectF FootnoteGroupItem::boundingRect() const {
  return QRectF();
}

void FootnoteGroupItem::paint(QPainter *, const QStyleOptionGraphicsItem *,
			 QWidget *) {
}

void FootnoteGroupItem::moveTo(double y) {
  foreach (FootnoteItem *c, children<FootnoteItem>()) {
    if (c->data()->y0() != y) {
      if (isWritable())
        c->data()->setY0(y);
      else
        c->data()->sneakilySetY0(y);
    }
    y += c->data()->height();
  }
  resetPosition();
}

void FootnoteGroupItem::childChanged() {
  emit heightChanged();
}
