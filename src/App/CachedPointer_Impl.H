// CachedPointer_Impl.H

#ifndef CACHEDPOINTER_IMPL_H

#define CACHEDPOINTER_IMPL_H
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
  }
  CachedPointer(CachedPointer<T> const &p) {
    p.cntr->ref();
    objp = p.objp;
    cntr = p.cntr;
  }
  virtual ~CachedPointer() {
    deref();
  }
  void clear() {
    /* Makes this CachedPointer points to a null pointer, deleting
       the original contents if only one (or no) reference to it remains.
       Returns true if zero references remain.
    */
    int newValue = cntr->fetchAndAddOrdered(-1) - 1;
    if (newValue<=1) {
      T *obj = objp->fetchAndStoreOrdered(0);
      if (obj)
	obj->deleteLater();
    }
    if (newValue<=0) {
      delete cntr;
      delete objp;
    }
    objp = new QAtomicPointer<T>(0);
    cntr = new QAtomicInt(1);
  }
  operator bool() const {
    return moreThanOne() && pointer()!=0;
  }
  T &operator*() const {
    ASSERT(moreThanOne());
    return *pointer();
  }
  T *operator->() const {
    return moreThanOne() ? pointer() : 0;
  }
  T *obj() const {
    return moreThanOne() ? pointer() : 0;
  }
  CachedPointer<T> &operator=(CachedPointer<T> const &p) {
    if (p.objp == objp) {
      qDebug() << "CachedPointer " << this << ": identity copy";
      return *this;
    }
    p.cntr->ref();
    deref();
    objp = p.objp;
    cntr = p.cntr;
    return *this;
  }
private:
  void deref() {
    int newValue = cntr->fetchAndAddOrdered(-1) - 1;
    if (newValue<=1) {
      T *obj = objp->fetchAndStoreOrdered(0);
      if (obj)
	obj->deleteLater();
    }
    if (newValue<=0) {
      delete cntr;
      delete objp;
    }
  }
  bool moreThanOne() const {
#if QT_VERSION >= 0x050000
    return cntr->load()>1;
#else
    return *cntr > 1;
#endif
  }
  T *pointer() const {
#if QT_VERSION >= 0x050000
    return objp->load();
#else
    return *objp;
#endif
  }    
private:
  QAtomicPointer<T> *objp;
  QAtomicInt *cntr;
};

#endif
