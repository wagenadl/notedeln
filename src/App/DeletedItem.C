// DeletedItem.C

#include "DeletedItem.H"

DeletedItem::DeletedItem(Item *item) {
  Q_ASSERT(item);
  data = item->data();
  Q_ASSERT(data);
  parentItem = item->itemParent();
  Q_ASSERT(parentItem);
  QObject *parentObject = parentItem;
  Q_ASSERT(parentObject);  
  connect(parentObject, SIGNAL(destroyed()), this, SLOT(parentDestroyed()));
}

DeletedItem::~DeletedItem() {
}

DeletedItem *DeletedItem::takeFromParent(Item *item) {
  DeletedItem *dd = new DeletedItem(item);
  item->data()->parent()->takeChild(item->data());
  item->itemParent()->deleteChild(item);
  dd->parentItem->childGeometryChanged();
  return dd;
}

bool DeletedItem::isRestored() const {
  if (isOrphaned())
    return false;
  return parentItem->data()->children<Data>().contains(data);
}

Item *DeletedItem::restoreToParent() {
  Q_ASSERT(!isOrphaned());
  Q_ASSERT(!isRestored());
  parentItem->data()->addChild(data);
  Item *item = Item::create(data, parentItem);
  parentItem->childGeometryChanged();
  return item;
}

void DeletedItem::parentDestroyed() {
  parentItem = 0;
}

bool DeletedItem::isOrphaned() const {
  return parentItem == 0;
}

