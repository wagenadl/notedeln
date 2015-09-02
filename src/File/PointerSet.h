// File/PointerSet.h - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// PointerSet.H

#ifndef POINTERSET_H

#define POINTERSET_H

#include <QObject>
#include <QSet>

class PointerSet: public QObject {
  Q_OBJECT;
public:
  void insert(QObject *);
  bool isEmpty() const;
  template <class T> QList<T*> toList() const {
    QList<T*> lst;
    foreach (QObject *p, data) {
      T *p1 = dynamic_cast<T*>(p);
      if (p1)
        lst.append(p1);
    }
    return lst;
  }
  void clear();
public slots:
  void remove(QObject *);
signals:
  void emptied();
protected:
  QSet<QObject*> data;
};

#endif
