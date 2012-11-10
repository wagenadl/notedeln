// DeletedItem.H

#ifndef DELETEDITEM_H

#define DELETEDITEM_H

#include "Data.H"
#include "Item.H"

class DeletedItem: public QObject {
  Q_OBJECT;
public:
  virtual ~DeletedItem();
  static DeletedItem *takeFromParent(Item *item);
  Item *restoreToParent();
  bool isOrphaned() const;
  bool isRestored() const; // true if already restored
private slots:
  void parentDestroyed();
private:
  DeletedItem(Item *item);
private:  
  Data *data;
  Item *parentItem;
};

#endif
