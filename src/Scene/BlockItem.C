// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "PageScene.H"

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

