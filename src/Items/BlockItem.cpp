// Items/BlockItem.cpp - This file is part of eln

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

// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "EntryScene.H"
#include "FootnoteItem.H"
#include "FootnoteData.H"
#include "Mode.H"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

BlockItem::BlockItem(BlockData *data, Item *parent):
  Item(data, parent) {
  bs = 0;

  foreach (FootnoteData *fd, data->children<FootnoteData>()) 
    newFootnote(fd);
}

BlockItem::~BlockItem() {
}

FootnoteItem *BlockItem::newFootnote(FootnoteData *fd) {
  FootnoteItem *fni = new FootnoteItem(fd, 0);
  if (fd->height()==0) 
    fni->sizeToFit();
  fni->resetPosition();
  connect(fni, SIGNAL(heightChanged()),
	  this, SIGNAL(heightChanged()));
  foots << fni;
  return fni;
}

BlockItem const *BlockItem::ancestralBlock() const {
  return this;
}

BlockItem *BlockItem::ancestralBlock() {
  return this;
}

void BlockItem::refTextChange(QString olds, QString news) {
  qDebug() << "BlockItem" << this<< "refTextChange";
  QSet<FootnoteItem *> dropset;
  foreach (FootnoteItem *fni, foots) {
    if (fni==0)
      continue;
    if (fni->data()->tag()==olds) {
      // Found a footnote that is affected by this.
      if (news.isEmpty())
	dropset.insert(fni);
      else
	fni->setTagText(news);
    }
  }
  foreach (FootnoteItem *fni, dropset) {
    foots.removeAll(fni);
    FootnoteData *fnd = fni->data();
    fni->deleteLater();
    data()->deleteChild(fnd);
  }
  if (!dropset.isEmpty())
    emit heightChanged();

  // we should remove null notes, but it's not important
}

void BlockItem::resetPosition() {
  QRectF r = mapRectToScene(netBounds());
  double y0 = data()->y0();
  if (y0!=r.top())
    setPos(pos().x(), pos().y() + y0 - r.top());
}

void BlockItem::setBaseScene(BaseScene *b) {
  bs = b;
}

BaseScene *BlockItem::baseScene() {
  return bs;
}

void BlockItem::makeWritable() {
  Item::makeWritable();
  foreach (FootnoteItem *fni, foots) 
    if (fni->parent() != this)
      fni->makeWritable();
}

QList<FootnoteItem *> BlockItem::footnotes() {
  QList<FootnoteItem *> r;
  foreach (FootnoteItem *fni, foots)
    if (fni)
      r << fni;
  return r;
}
