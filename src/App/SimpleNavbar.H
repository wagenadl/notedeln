// App/SimpleNavbar.H - This file is part of eln

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

// SimpleNavbar.H

#ifndef SIMPLENAVBAR_H

#define SIMPLENAVBAR_H

#include "Toolbar.H"

class SimpleNavbar: public Toolbar {
  Q_OBJECT;
public:
  SimpleNavbar(QGraphicsScene *scene);
  virtual ~SimpleNavbar();
signals:
  void goRelative(int); // forward by n pages (n may be negative)
protected:
  virtual void doLeftClick(QString);
};

#endif
