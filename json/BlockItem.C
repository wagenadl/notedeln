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

void BlockItem::checkBbox() {
  QRectF newBbox = netBoundingRect();
  if (newBbox == oldBbox)
    return;

  if (newBbox.left()!=oldBbox.left() || newBbox.right()!=oldBbox.right())
    emit hboxChanged();
  if (newBbox.top()!=oldBbox.top() || newBbox.bottom()!=oldBbox.bottom())
    emit vboxChanged();

  oldBbox = newBbox;
}

void BlockItem::checkVbox() {
  if (!mainChild_)
    return;

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
  return mainChild_
    ? mainChild_->mapRectToParent(mainChild_->boundingRect())
    : QRectF();
}

void BlockItem::setMainChild(QGraphicsItem *g) {
  mainChild_ = g;
  if (g)
    oldBbox = netBoundingRect();
}

QGraphicsItem *BlockItem::mainChild() const {
  return mainChild_;
}

void BlockItem::paint(QPainter *p,
		      const QStyleOptionGraphicsItem *o,
		      QWidget *w) {
  if (mainChild_)
    mainChild_->paint(p, o, w);
  // should include notes as well
}
