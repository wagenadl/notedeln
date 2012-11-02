// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "PageScene.H"
#include "FootnoteGroupItem.H"
#include "FootnoteItem.H"
#include "FootnoteData.H"

#include <QDebug>

BlockItem::BlockItem(BlockData *data, Item *parent):
  QGraphicsObject(gi(parent)), Item(data, *this), data_(data) {
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

void BlockItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  mousePress(e);
}

void BlockItem::refTextChange(QString olds, QString news) {
  qDebug() << "BlockItem::refTextChange" << olds << news;
  foreach (FootnoteData *fnd, data()->children<FootnoteData>()) {
    if (fnd->tag()==olds) {
      qDebug() << "BlockItem:refTextChange: found data";
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
		  qDebug() << "Deleting footnote item";
		  fng->deleteChild(fni);
		} else {
		  qDebug() << "Updating footnote item";
		  fni->setTagText(news);
		}
		fng->restack();
	      }
	    }
	  }
	}
      }
	
      if (news.isEmpty()) {
	qDebug() << "Deleting data";
	data()->deleteChild(fnd);
      }
    }
  }
}
