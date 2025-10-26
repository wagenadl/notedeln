// App/CUI.h - This file is part of NotedELN

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

// CUI.h

#ifndef CUI_H

#define CUI_H

#include <QString>

class CUI {
public:
  static CUI *instance();
public:
  bool match(QString) const;
  QString current() const;
  static void globallyDisable();
private:
  CUI();
  QString adjustID(QString, qint64, qint64);
  QString adjustID(QString, qint64);
  friend class App;
  QString c, c1, c2;
  static bool &nocui();
};

#endif
