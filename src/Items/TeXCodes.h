// Items/TeXCodes.H - This file is part of NotedELN

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

// TeXCodes.H

#ifndef TEXCODES_H

#define TEXCODES_H

#include <QMap>
#include <QString>

class TeXCodes {
public:
  static bool contains(QString);
  static bool onlyExplicit(QString);
  static QString map(QString);
private:
  static void ensure();
  static QMap<QString, QString> map_;
  static QMap<QString, QString> nox_;
};

#endif
