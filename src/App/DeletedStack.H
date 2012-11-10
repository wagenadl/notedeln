// DeletedStack.H

#ifndef DELETEDSTACK_H

#define DELETEDSTACK_H

#include "DeletedItem.H"
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
