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
  Item(data, parent) {
}

BlockItem::~BlockItem() {
}

BlockItem const *BlockItem::ancestralBlock() const {
  return this;
}

BlockItem *BlockItem::ancestralBlock() {
  return this;
}

void BlockItem::refTextChange(QString olds, QString news) {
  qDebug() << "BlockItem" << this<< "refTextChange";
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
                fni->setTagText(news);
		if (news.isEmpty()) 
		  fni->deleteLater();
	      }
	    }
	  }
	}
      }
	
      if (news.isEmpty()) 
	data()->deleteChild(fnd);
    }
  }
}

void BlockItem::resetPosition() {
  QRectF r = mapRectToScene(netBounds());
  double y0 = data()->y0();
  if (y0!=r.top())
    setPos(pos().x(), pos().y() + y0 - r.top());
}

