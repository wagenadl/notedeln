// CachedPointer.H

#ifndef CACHEDPOINTER_H

#define CACHEDPOINTER_H

#include <QAtomicInt>
#include <QAtomicPointer>

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
  CachedPointer();
  explicit CachedPointer(T *obj);
  CachedPointer(CachedPointer<T> const &p);
  virtual ~CachedPointer();
  void clear();
  operator bool() const;
  T &operator*() const;
  T *operator->() const;
  T *obj() const;
  CachedPointer<T> &operator=(CachedPointer<T> const &p);
private:
  void deref();
private:
  QAtomicPointer<T> *objp;
  QAtomicInt *cntr;
};

#endif
