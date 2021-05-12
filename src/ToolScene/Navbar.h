// App/Navbar.H - This file is part of NotedELN

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

// Navbar.H

#ifndef NAVBAR_H

#define NAVBAR_H

#include "Toolbar.h"

class Navbar: public Toolbar {
  Q_OBJECT;
public:
  enum PageType {
    FrontPage,
    TOC,
    Entry,
    LastEntry,
    EmptyLastEntry
  };
public:
  Navbar(QGraphicsItem *parent);
  virtual ~Navbar();
public slots:
  void showHelp();
  void setPageType(PageType);
  void hidePlus();
  void showPlus();
signals:
  void goTOC(Qt::KeyboardModifiers);
  void goFind();
  void goEnd(Qt::KeyboardModifiers);
  void goRelative(int, Qt::KeyboardModifiers);
  // forward by n pages (n may be negative)
  void goNew(Qt::KeyboardModifiers);
  void goPrint();
protected:
  virtual void doLeftClick(QString, Qt::KeyboardModifiers);
private:
  ToolItem *ti_n1, *ti_n10, *ti_end, *ti_plus, *ti_p1, *ti_p10;
};

#endif
