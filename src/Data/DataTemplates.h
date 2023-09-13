// Data/DataTemplates.H - This file is part of NotedELN

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

// DataTemplates.H

#ifndef DATATEMPLATES_H

#define DATATEMPLATES_H

#include <QList>

template <class T> QList<T *> Data::children() const {
  QList<T *> res;
  for (QList<Data*>::const_iterator i=children_.begin();
       i!=children_.end(); ++i) {
    T *c = dynamic_cast<T*>(*i);
    if (c)
      res.append(c);
  }
  return res;
}

template <class T> T *Data::firstChild() const {
  for (QList<Data*>::const_iterator i=children_.begin();
       i!=children_.end(); ++i) {
    T *c = dynamic_cast<T*>(*i);
    if (c)
      return c;
  }
  return 0;
}

template <class T> T *Data::deepCopy(T const *data) {
  T *copy = new T(0);
  copy->load(data->save());
  return copy;
}

#endif
