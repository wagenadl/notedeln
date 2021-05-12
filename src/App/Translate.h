// App/Translate.h - This file is part of NotedELN

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

// Translate.H

#ifndef TRANSLATE_H

#define TRANSLATE_H

#include <QString>
#include <QMap>

class Translate {
public:
  static QString _(QString key);
  static QString language();
  static void setLanguage(QString l=""); // empty for default from OS
  // setLanguage must be called _before_ the first call to _().
private:
  static QMap<QString, QString> const &map();
  static QString &lang();
};

#endif
