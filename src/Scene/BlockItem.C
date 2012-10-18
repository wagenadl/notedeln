// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "PageScene.H"

BlockItem::BlockItem(BlockData *data, PageScene *parent):
  QGraphicsObject(0), Item(data, this), data_(data) {
  parent->addItem(this);
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
