// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "PageScene.H"
#include "FootnoteGroupItem.H"
#include "FootnoteItem.H"
#include "FootnoteData.H"
#include "Mode.H"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

BlockItem::BlockItem(BlockData *data, Item *parent):
  Item(data, parent), data_(data) {
  bbLocked = false;
}

BlockItem::~BlockItem() {
}

BlockData *BlockItem::data() {
  return data_;
}

QRectF BlockItem::boundingRect() const {
  return bbox;
}

void BlockItem::sizeToFit() {
  if (bbLocked)
    return;
  
  QRectF newBbox = fittedRect();
  bool chg = newBbox != bbox;
  // newBbox.top()!=bbox.top() || newBbox.bottom()!=bbox.bottom();
  bbox = newBbox;
  if (chg) {
    prepareGeometryChange();
    qDebug() << "BlockItem"<<this<<"emitting vboxChanged";
    emit boundsChanged();
  }
}

QRectF BlockItem::fittedRect() const {
  return netChildBoundingRect();
}

void BlockItem::lockBounds() {
  bbLocked = true;
}

void BlockItem::unlockBounds() {
  bbLocked = false;
  sizeToFit();
}

BlockItem const *BlockItem::ancestralBlock() const {
  return this;
}

BlockItem *BlockItem::ancestralBlock() {
  return this;
}

void BlockItem::refTextChange(QString olds, QString news) {
  foreach (FootnoteData *fnd, data()->children<FootnoteData>()) {
    if (fnd->tag()==olds) {
      // Found a footnote that is affected by this. Let's find the
      // corresponding item.
      QGraphicsScene *s = scene();
      if (s) {
	foreach (QGraphicsItem *i, s->items()) {
	  FootnoteGroupItem *fng = dynamic_cast<FootnoteGroupItem*>(i);
	  if (fng) {
	    foreach (FootnoteItem *fni, fng->children<FootnoteItem>()) {
	      if (fni->data()==fnd) {
		// found the note!
		if (news.isEmpty()) {
		  fni->deleteLater();
		} else {
		  fni->setTagText(news);
		}
		fng->restack();
	      }
	    }
	  }
	}
      }
	
      if (news.isEmpty()) {
	data()->deleteChild(fnd);
      }
    }
  }
}


