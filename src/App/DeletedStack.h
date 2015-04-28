// App/DeletedStack.H - This file is part of eln

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

// DeletedStack.H

#ifndef DELETEDSTACK_H

#define DELETEDSTACK_H

#include "DeletedItem.h"
#include <QList>

class DeletedStack: public QObject {
  Q_OBJECT;
public:
  DeletedStack(QObject *parent=0);
  virtual ~DeletedStack();
  bool grabIfRestorable(Item *item); // this takes the item away from its parent
  bool isEmpty(); // orphaned data do not make this return false
  bool restoreTop();
  // Restores topmost item, if any, and pops it off the stack. False if none.
private:
  void cleanup(); // pop orphans from top of stack
private:
  QList<DeletedItem*> stack;
};

#endif
