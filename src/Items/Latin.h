// Items/Latin.H - This file is part of NotedELN

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

// Latin.H

#ifndef LATIN_H

#define LATIN_H

#include <QMap>
#include <QSet>
#include <QString>
#include <QVariant>

class Latin {
public:
  static QSet<QString> const &normal(QString);
  static QSet<QString> const &abbrev(QString);
private:
  static QVariantMap const &json();
  static QMap<QString, QSet<QString> > load(QString);
  static QMap<QString, QSet<QString> > const &normal();
  static QMap<QString, QSet<QString> > const &abbrev();
};

#endif
