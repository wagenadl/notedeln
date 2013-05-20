// DeletedItem.C

#include "DeletedItem.H"
#include "Assert.H"
#include "BlockItem.H"

DeletedItem::DeletedItem(Item *item) {
  ASSERT(item);
  data = item->data();
  ASSERT(data);
  parentItem = item->parent();
  ASSERT(parentItem);
  connect(parentItem, SIGNAL(destroyed()), this, SLOT(parentDestroyed()));
}

DeletedItem::~DeletedItem() {
}

DeletedItem *DeletedItem::takeFromParent(Item *item) {
  ASSERT(item);
  BlockItem *ancestor = item->ancestralBlock();
  DeletedItem *dd = new DeletedItem(item);
  item->data()->parent()->takeChild(item->data());
  item->deleteLater();
 if (ancestor)
    ancestor->sizeToFit();
   return dd;
}

bool DeletedItem::isRestored() const {
  if (isOrphaned())
    return false;
  return parentItem->data()->children<Data>().contains(data);
}

Item *DeletedItem::restoreToParent() {
  ASSERT(!isOrphaned());
  ASSERT(!isRestored());
  parentItem->data()->addChild(data);
  Item *item = Item::create(data, parentItem);
  item->makeWritable();
  BlockItem *ancestor = item->ancestralBlock();
  if (ancestor)
    ancestor->sizeToFit();
  return item;
}

void DeletedItem::parentDestroyed() {
  parentItem = 0;
}

bool DeletedItem::isOrphaned() const {
  return parentItem == 0;
}

