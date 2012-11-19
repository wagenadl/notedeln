// Item.H

#ifndef ITEM_H

#define ITEM_H

#include "Data.H"
#include <QGraphicsObject>
#include <QObject>
#include <Qt>
#include "Style.H"

class Item: public QGraphicsObject {
public:
  Item(Data *d, Item *parent);
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
  void addChild(Item *);
  bool deleteChild(Item *); // this uses deleteLater if child is an object
  class GfxNoteItem *newNote(QPointF p0, QPointF p1, bool late);
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
  void acceptModifierChanges(bool acc=true);
protected:
  bool moveModPressed() const;
  class GfxNoteItem *createNote(QPointF p0, bool late);
protected:
  static Qt::CursorShape defaultCursor();
  static Qt::KeyboardModifiers moveModifiers(); // mask of modifier keys ...
  static Qt::MouseButton moveButton(); // ... and mouse button that should
                                       // trigger a move
  static class ModSnooper *modSnooper();
private:
  static QMap<QString, Item *(*)(Data *, Item *)> &creators();
private:
  Item(Item const &); // this must never be called
private:
  Data *d;
  QList<Item *> children_; // do I really want these?
  bool extraneous;
  bool brLocked;
  QRectF brCache;
  bool writable;
};

#endif
