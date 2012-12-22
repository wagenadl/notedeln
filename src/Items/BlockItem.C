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
}

BlockItem::~BlockItem() {
}

BlockData *BlockItem::data() {
  return data_;
}

void BlockItem::checkVbox() {
  QRectF newBbox = netBoundingRect();

  if (newBbox.top()==oldBbox.top() && newBbox.bottom()==oldBbox.bottom())
    return;
  
  emit vboxChanged();
  oldBbox = newBbox;
}

void BlockItem::resetBbox() {
  oldBbox = netBoundingRect();
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
	    foreach (FootnoteItem *fni, fng->itemChildren<FootnoteItem>()) {
	      if (fni->data()==fnd) {
		// found the note!
		if (news.isEmpty()) {
		  fng->deleteChild(fni);
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


BlockItem const *BlockItem::ancestralBlock(Item const *i) {
  while (i) {
    BlockItem const *bi = dynamic_cast<BlockItem const*>(i);
    if (bi)
      return bi;
    i = i->itemParent();
  }
  return 0;
}

BlockItem *BlockItem::ancestralBlock(Item *i) {
  while (i) {
    BlockItem *bi = dynamic_cast<BlockItem*>(i);
    if (bi)
      return bi;
    i = i->itemParent();
  }
  return 0;
}
