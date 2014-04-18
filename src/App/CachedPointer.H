// CachedPointer.H

#ifndef CACHEDPOINTER_H

#define CACHEDPOINTER_H

#include <QAtomicInt>
#include <QAtomicPointer>
#include "Assert.H"
#include <QDebug>

template <typename T> class CachedPointer {
  /* A CachedPointer is like a QSharedPointer but with a twist: The object
     pointed to is deleted when the penultimate pointer goes out of scope.
     This means that a cache of pointers can be maintained from which the
     object can be checked out multiple times and when the last checked out
     pointer goes out of scope, the object is deleted and the pointer in the
     cache is invalidated. This obviously works iff there is precisely one
     copy of the pointer in the cache.
     It is imperative that the cached copy is never directly used as a
     plain pointer.
  */
public:
  CachedPointer() {
    objp = new QAtomicPointer<T>(0);
    cntr = new QAtomicInt(1);
  }
  explicit CachedPointer(T *obj) {
    objp = new QAtomicPointer<T>(obj);
    cntr = new QAtomicInt(1);
    qDebug() << "New CachedPointer " << this << " for " << objp << " : " << obj;
  }
  CachedPointer(CachedPointer<T> const &p) {
    p.cntr->ref();
    objp = p.objp;
    cntr = p.cntr;
    qDebug() << "CachedPointer " << this << " for " << objp << " : "  << *objp << " now has " << int(*p.cntr) << " refs";
  }
  virtual ~CachedPointer() {
    qDebug() << "~CachedPointer " << this;
    if (clear()) {
      qDebug() << "  CachePointer deleting self";
      // Final copy being deleted: delete the references
      delete cntr;
      delete objp;
    }
  }
  bool clear() {
    /* Makes this CachedPointer points to a null pointer, deleting
       the original contents if only one (or no) reference to it remains.
       Returns true if zero references remain.
    */
    int newValue = cntr->fetchAndAddOrdered(-1) - 1;
    qDebug() << "CachedPointer " << this << " for " << objp << " : " << *objp << " now has " << int(*cntr) << " refs";
    if (newValue<=1) {
      T *obj = objp->fetchAndStoreOrdered(0);
      qDebug() << "  CachePointer deleting " << obj;
      if (obj)
	obj->deleteLater();
      qDebug() << " deleted objp now " << objp << " : " << *objp;
    }
    return newValue<=0;
  }
  operator bool() const {
    return *cntr>1 && *objp!=0;
  }
  T &operator*() const {
    ASSERT(*cntr > 1);
    return **objp;
  }
  T *operator->() const {
    return *cntr>1 ? *objp : 0;
  }
  T *obj() const {
    return *cntr>1 ? *objp : 0;
  }
  CachedPointer<T> &operator=(CachedPointer<T> const &p) {
    p.cntr->ref();

    /* Now, our old contents is effectively being deleted. */
    if (clear()) {
      // Final copy being deleted: delete the references
      delete cntr;
      delete objp;
    }

    objp = p.objp;
    cntr = p.cntr;
    return *this;
  }
private:
  QAtomicPointer<T> *objp;
  QAtomicInt *cntr;
};

#endif
