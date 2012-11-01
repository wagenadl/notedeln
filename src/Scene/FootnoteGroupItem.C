// FootnoteGroup.C

#include "FootnoteGroupItem.H"
#include "BlockData.H"
#include "FootnoteData.H"
#include "FootnoteItem.H"
#include "PageScene.H"

FootnoteGroupItem::FootnoteGroupItem(BlockData *data, PageScene *parent):
  QGraphicsObject(0), Item(data, this), data(data) {
  parent->addItem(this);

  int y = 0;
  foreach (FootnoteData *fd, data->children<FootnoteData>()) {
    FootnoteItem *fni = new FootnoteItem(fd, this);
    addChild(fni);
    fni->setPos(0, y);
    y += fni->childrenBoundingRect().height();
  }  
}

FootnoteGroupItem::~FootnoteGroupItem() {
}

void FootnoteGroupItem::makeWritable() {
  foreach (Item *c, Item::children<FootnoteItem>())
    c->makeWritable();
}

QRectF FootnoteGroupItem::boundingRect() const {
  return QRectF();
}

void FootnoteGroupItem::paint(QPainter *, const QStyleOptionGraphicsItem *,
			 QWidget *) {
}

