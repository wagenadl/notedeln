// Item.H

#ifndef ITEM_H

#define ITEM_H

#include "Data.H"
#include <QGraphicsItem>
#include <QObject>
#include <Qt>

class Item {
public:
  Item(Data *d, QGraphicsItem *me);
  virtual ~Item();
  Data *data();
  Item *itemParent() const;
  class PageScene *pageScene() const;
  QRectF netBoundingRect() const; // potentially cached if locked
  QRectF netSceneRect() const; // in scene coords
  bool isExtraneous() const;
  void setExtraneous(bool e=true); // extraneous items are not used for
                                   // calculations of netBoundingRect
  void lockBounds();   // } children call these to prevent 
  void unlockBounds(); // } geom. update during drags
  QRectF cachedBounds() const; // null if not locked
  virtual void childGeometryChanged(); // children must call this directly
  bool childless() const;
  QList<Item*> const &allChildren() const;
  template<class T> QList<T*> children() const {
    QList<T *> res;
    for (QList<Item*>::const_iterator i=children_.begin();
	 i!=children_.end(); ++i) {
      T *c = dynamic_cast<T*>(*i);
      if (c)
	res.append(c);
    }
    return res;
  }
  void addChild(Item *);
  bool deleteChild(Item *);
  static Item *create(Data *d, Item *parent=0);
  static QGraphicsItem *gi(Item *);
  static QObject *obj(Item *);
  template <class DT, class IT> class Creator {
  public:
    Creator<DT, IT>(QString typ) {
      Item::creators()[typ] = &create;
    }
    static Item *create(Data *d, Item *parent=0) {
      return new IT(dynamic_cast<DT*>(d), parent);
    }
  };
public:
  static Qt::CursorShape defaultCursor();
  static Qt::KeyboardModifiers moveModifiers(); // mask of modifier keys ...
  static Qt::MouseButton moveButton(); // ... and mouse button that should
                                       // trigger a move
private:
  static QMap<QString, Item *(*)(Data *, Item *)> &creators();
private:
  Data *d;
  QGraphicsItem *me;
  QList<Item *> children_;
  bool extraneous;
  bool brLocked;
  QRectF brCache;
};

#endif
