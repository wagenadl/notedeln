// Book/Catalog.h - This file is part of NotedELN

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

// Catalog.h

#ifndef CATALOG_H

#define CATALOG_H

#include <QMap>
#include <QMultiMap>
#include <QString>
#include <QDateTime>
#include <QStringList>

class Catalog {
  // Catalog of files in the pages/ folder
public:
  Catalog(QString pgdir);
  QString path() const { return pgdir; }
  bool isValid() const { return ok; } // dir could be read
  bool isClean() const; // no errors and no duplicates
  QMultiMap<int, QString> const &pageToFileMap() const { return pg2file; }
  QDateTime fileMod(QString) const;
  bool hasNotes(QString) const;
  QDateTime noteDirMod(QString) const;
  QStringList errors() const { return err; }
private:
  QString pgdir;
  QMultiMap<int, QString> pg2file;
  QMap<QString, QDateTime> filemods;
  QMap<QString, QDateTime> notemods;
  bool ok;
  QStringList err;
};

#endif
