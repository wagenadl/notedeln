// Item.H

#ifndef ITEM_H

#define ITEM_H

#include "Data.H"
#include <QGraphicsItem>
#include <QObject>
#include <Qt>
#include "Style.H"

class Item {
public:
  Item(Data *d, QGraphicsItem &me);
  virtual ~Item();
  Data *data();
  Item *itemParent() const;
  class PageScene *pageScene() const;
  QRectF netBoundingRect() const; // potentially cached if locked
  QRectF netSceneRect() const; // in scene coords
  bool isExtraneous() const;
  void setExtraneous(bool e=true); // extraneous items are not used for
                                   // calculations of netBoundingRect
  bool isWritable() const;
  virtual void makeWritable();
  Style const &style() const;
  QVariant style(QString) const;
  bool childless() const;
  template<class T> QList<T*> itemChildren() const {
    QList<T *> res;
    for (QList<Item*>::const_iterator i=children_.begin();
	 i!=children_.end(); ++i) {
      T *c = dynamic_cast<T*>(*i);
      if (c)
	res.append(c);
    }
    return res;
  }
public:
  static Item *create(Data *d, Item *parent=0);
  static QGraphicsItem *gi(Item *); // null pointer ok
  static QObject *obj(Item *);
  void addChild(Item *);
  bool deleteChild(Item *); // this uses deleteLater if child is an object
  QGraphicsItem *gi();
  QObject *obj();
  class GfxNoteItem *newNote(QPointF p0, QPointF p1);
  virtual bool abandonNote(class GfxNoteItem *);
public: // but only for use in derived class source files
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
  void lockBounds();   // } children call these to prevent 
  void unlockBounds(); // } geom. update during drags
  QRectF cachedBounds() const; // null if not locked
  virtual void childGeometryChanged(); // children must call this directly
protected:
  void acceptModifierChanges();
  bool moveModPressed() const;
  bool mousePress(QGraphicsSceneMouseEvent *); // must only be called
  // from within mousePressEvent. Returns true if accepted
protected:
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
  bool writable;
};

#endif
