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

#include "BlockItem.h"
#include "BlockData.h"
#include "TextItem.h"
#include "EntryScene.h"
#include "FootnoteItem.h"
#include "FootnoteData.h"
#include "Mode.h"

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
  //  fni->setBaseScene(baseScene()); // is this a good idea?
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

static int countReferencesIn(Data const *d0, QString txt) {
  int cnt = 0;
  qDebug() << "refs in" << d0 << txt;
  foreach (Data *d, d0->allChildren()) {
    MarkupData *md = dynamic_cast<MarkupData *>(d);
    if (md) {
      qDebug() << "ref" << md->text() << md->styleName(md->style())
	       << (md->text() == txt);
      if (md->style()==MarkupData::FootnoteRef && md->text() == txt)
	cnt ++;
    } else {
      cnt += countReferencesIn(d, txt);
    }
  }
  return cnt;
}

int BlockItem::countReferences(QString txt) const {
  // count references in any child or grand child
  if (txt.isEmpty())
    return 0;
  return countReferencesIn(data(), txt);
}
  
void BlockItem::refTextChange(QString olds, QString news) {
  int nOldRefs = countReferences(olds);
  int nNewRefs = countReferences(news);
  qDebug() << "BI: reftxtchg" << olds << news << nOldRefs << nNewRefs;
  // nOldRefs is *other* refs; the change must already have been made
  // nNewRefs is other refs plus the one that just changed
  ASSERT(news.isEmpty() || nNewRefs>0);

  QSet<FootnoteItem *> affected;
  foreach (FootnoteItem *fni, foots) {
    if (fni && fni->data()->tag()==olds)
      affected.insert(fni);
  }

  if (affected.isEmpty()) {
    qDebug() << "No footnote matching this reference text.";
    // we could have an assertion instead
    return;
  }

  if (news.isEmpty()) {
    /* We will delete the footnotes if there are no references left.
       Otherwise, we'll leave them in peace. */
    if (nOldRefs==0) {
      foreach (FootnoteItem *fni, affected) {
	foots.removeAll(fni);
	FootnoteData *fnd = fni->data();
	fni->deleteLater();
	data()->deleteChild(fnd);
      }
      emit heightChanged();
    }
  } else {
    /* We can duplicate the footnote if there are multiple references,
       duplicating the text, and changing the ref only on the new note. */
    if (nOldRefs>0) {
      if (affected.size()>1) {
	/* Multiple footnotes, same tags. We'll just muck with one at random.
	   This is not good, but I don't know what else I could do.
	*/
	FootnoteItem *fni = *affected.begin();
	fni->setTagText(news);
      } else {
	FootnoteItem *fni0 = *affected.begin();
	FootnoteData *fnd1 = new FootnoteData(data());
	fnd1->setTag(news);
	fnd1->text()->setText(fni0->data()->text()->text());
	FootnoteItem *fni1 = newFootnote(fnd1);
	fni1->makeWritable();
	emit heightChanged();
      }
    } else {
      // just change the tags
      foreach (FootnoteItem *fni, affected) 
	fni->setTagText(news);
    }
  }

  // I do not understand this old comment:
  // "We should remove null notes, but it's not important."
}

void BlockItem::resetPosition() {
  QRectF r = mapRectToScene(netBounds());
  double y0 = data()->y0();
  if (y0!=r.top())
    setPos(pos().x(), pos().y() + y0 - r.top());
}

void BlockItem::setBaseScene(BaseScene *b) {
  bs = b;
  // for (FootnoteItem *fni: footnotes())
  //   fni->setBaseScene(b);
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

QPointF BlockItem::findRefText(QString s) {
  QPointF p = findRefTextIn(s, this);
  return p;
}

QPointF BlockItem::findRefTextIn(QString s, Item *i) {
  TextItem *ti = dynamic_cast<TextItem *>(i);
  if (ti) {
    return ti->posToPoint(ti->data()->offsetOfFootnoteTag(s));
  } else {
    foreach (Item *c, i->allChildren()) {
      QPointF p = findRefTextIn(s, c);
      if (!p.isNull()) {
	return c->mapToParent(p);
      }
    }
  }
  return QPointF();
}

double BlockItem::visibleHeight() const {
  return data()->height();
}
