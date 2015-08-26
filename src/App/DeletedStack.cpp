// App/DeletedStack.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// DeletedStack.C

#include "DeletedStack.h"
#include "GfxData.h"
#include "GfxNoteItem.h"
#include "TextItem.h"

#include <QDebug>

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
    item = item->parent();
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
