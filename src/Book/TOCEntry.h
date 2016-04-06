// Book/TOCEntry.H - This file is part of eln

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

// TOCEntry.H

#ifndef TOCENTRY_H

#define TOCENTRY_H

#include "Data.h"

class TOCEntry: public Data {
  Q_OBJECT;
  Q_PROPERTY(int startPage READ startPage WRITE setStartPage);
  Q_PROPERTY(QString title READ title WRITE setTitle);
  Q_PROPERTY(int sheetCount READ sheetCount WRITE setSheetCount);
  Q_PROPERTY(QDateTime seen READ lastSeen WRITE setLastSeen);
public:
  TOCEntry(Data *parent=0);
  virtual ~TOCEntry();
  // read properties
  int startPage() const;
  QString title() const;
  int sheetCount() const;
  QDateTime lastSeen() const;
  // write properties
  void setStartPage(int);
  void setTitle(QString);
  void setSheetCount(int);
  void setLastSeen(QDateTime const &);
private:
  int startPage_;
  QString title_;
  int sheetCount_;
  QDateTime seen_;
};

#endif
