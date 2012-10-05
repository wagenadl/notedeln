// BlockItem.C

#include "BlockItem.H"
#include "BlockData.H"
#include "PageScene.H"

BlockItem::BlockItem(BlockData *data, PageScene *parent):
  QGraphicsObject(0),
  data_(data) {
  mainChild_ = 0;
  parent->addItem(this);
}

BlockItem::~BlockItem() {
}

BlockData *BlockItem::data() {
  return data_;
}

QRectF BlockItem::contentsRect() const {
  if (mainChild_)
    return mainChild_->mapRectToParent(mainChild_->boundingRect());
  else
    return QRectF();
}

QRectF BlockItem::contentsSceneRect() const {
  return mapRectToScene(contentsRect());
}

void BlockItem::checkBbox() {
  if (!mainChild_)
    return;

  QRectF newBbox = contentsRect();
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

  QRectF newBbox = contentsRect();

  if (newBbox.top()==oldBbox.top() && newBbox.bottom()==oldBbox.bottom())
    return;
  
  emit vboxChanged();
  oldBbox = newBbox;
}

void BlockItem::setMainChild(QGraphicsItem *c) {
  mainChild_ = c;
  oldBbox = contentsRect();
}

QRectF BlockItem::boundingRect() const {
  return contentsRect();
  // should include notes as well
}

void BlockItem::paint(QPainter *p,
		      const QStyleOptionGraphicsItem *o,
		      QWidget *w) {
  if (mainChild_)
    mainChild_->paint(p, o, w);
  // should include notes as well
}
