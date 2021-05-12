// App/CachedPointer.h - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

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
  bool moreThanOne() const;
  T *pointer() const;
private:
  QAtomicPointer<T> *objp;
  QAtomicInt *cntr;
};

#endif
