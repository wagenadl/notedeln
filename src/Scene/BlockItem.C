// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "PageScene.H"

BlockItem::BlockItem(BlockData *data, PageScene *parent):
  QGraphicsObject(0),
  data_(data) {
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

QRectF BlockItem::netSceneRect() const {
  return mapRectToScene(netBoundingRect());
}

QRectF BlockItem::boundingRect() const {
  QRectF bb = netBoundingRect();
  // should include notes as well
  return bb;
}

QRectF BlockItem::netBoundingRect() const {
  return QRectF();
}

void BlockItem::resetBbox() {
  oldBbox = netBoundingRect();
}
