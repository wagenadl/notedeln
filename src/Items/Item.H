// Item.H

#ifndef ITEM_H

#define ITEM_H

#include "Data.H"
#include <QGraphicsObject>
#include <QObject>
#include <Qt>
#include "Style.H"
#include "Assert.H"

#define DATAACCESS(T) T const *data() const { return constCastData<T>(); } \
                      T *data() { return castData<T>(); }

class Item: public QGraphicsObject {
  Q_OBJECT;
public:
  Item(Data *d, Item *parent);
  virtual ~Item();
  virtual void deleteLater();
  DATAACCESS(Data);
  Item *parent() const;
  class EntryScene *pageScene() const;
  class Mode const *mode() const;
  virtual QRectF netBounds() const;
  virtual bool excludeFromNet() const;
  bool isWritable() const;
  virtual void makeWritable();
  virtual void makeWritableNoRecurse();
  Style const &style() const;
  QVariant style(QString) const;
  virtual class BlockItem const *ancestralBlock() const;
  virtual class BlockItem *ancestralBlock();
  QList<Item*> allChildren() const;
  template<class T> QList<T*> children() const {
    QList<T *> res;
    foreach (QGraphicsItem *c0, QGraphicsObject::childItems()) {
      T *c = dynamic_cast<T*>(c0);
      if (c)
 	res.append(c);
    }
    return res;
  }
  virtual void setScale(qreal);
public:
  static Item *create(Data *d, Item *parent=0);
  class GfxNoteItem *createNote(QPointF p0, bool late);
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
protected:
  static Qt::CursorShape defaultCursor();
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  template<class T> T *castData() {
    T *d1 = dynamic_cast<T *>(d);
    ASSERT(d1);
    return d1;
  }
  template<class T> T const *constCastData() const {
    T const *d1 = dynamic_cast<T const*>(d);
    ASSERT(d1);
    return d1;
  }
private:
  static QMap<QString, Item *(*)(Data *, Item *)> &creators();
private: // these must never be called
  Item(Item const &); // copy constructor
  QGraphicsObject *parentObject() const; // hide from QGraphicsObject
  QGraphicsItem *parentItem() const; // hide from QGraphicsObject
  QList<QGraphicsItem *> childItems() const; // hide from QGraphicsItem
  const QObjectList &children() const; // hide from QObject
  // QObject's parent() has been overridden
  void setParent(QObject *); // hide from QObject
private:
  Data *d;
  bool extraneous;
  bool writable;
};

#endif
