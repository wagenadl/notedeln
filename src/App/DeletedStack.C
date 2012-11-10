// DeletedStack.C

#include "DeletedStack.H"
#include "GfxData.H"
#include "GfxNoteItem.H"
#include "TextItem.H"

DeletedStack::DeletedStack(QObject *parent): QObject(parent) {
}

DeletedStack::~DeletedStack() {
}

bool DeletedStack::grabIfRestorable(Item *item) {
  /* "Restorable" normally means that the item must represent GfxData. (As
     in the case of GfxMarkItem, GfxImageItem, etc.)
     For the important case of a GfxNoteItem, the "itemAt" method would
     return the TextItem instead of the GfxNoteItem, so we correct for that.
   */
  if (!item)
    return false;
  if (dynamic_cast<TextItem*>(item)) {
    item = item->itemParent();
    if (!dynamic_cast<GfxNoteItem*>(item))
      return false;
  }
  if (!dynamic_cast<GfxData*>(item->data()))
    return false;
  DeletedItem *dd = DeletedItem::takeFromParent(item);
  dd->setParent(this);
  stack.append(dd);
  return true;
}

bool DeletedStack::isEmpty() {
  cleanup();
  return stack.isEmpty();
}

bool DeletedStack::restoreTop() {
  cleanup();
  if (stack.isEmpty())
    return false;
  DeletedItem *dd = stack.last();
  if (dd->isRestored())
    return false;
  dd->restoreToParent();
  delete dd;
  stack.pop_back();
  return true;
}

void DeletedStack::cleanup() {
  while (!stack.isEmpty()) {
    DeletedItem *t = stack.last();
    if (t->isOrphaned()) {
      delete t;
      stack.pop_back();
    } else {
      break;
    }
  }
}
