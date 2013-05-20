// Book/TOC.H - This file is part of eln

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

// TOC.H

#ifndef TOC_H

#define TOC_H

#include <QObject>
#include <QVariant>
#include <QDate>
#include "Data.H"
#include "DataFile.H"
#include "TOCEntry.H"

class TOC: public Data {
  Q_OBJECT;
public:
  TOC(Data *parent=0);
  virtual ~TOC();
  QMap<int, TOCEntry *> const &entries() const;
  TOCEntry *entry(int startPage) const; // assertion if not found
  TOCEntry *find(int page) const; // returns entry containing the numbered page
                                  // ... or 0 if none does.
  bool contains(int startPage) const;
  TOCEntry *addEntry(class EntryData *data);
  bool deleteEntry(TOCEntry *);
  int newPageNumber() const;
  bool isLast(TOCEntry const *) const;
  void setBook(class Notebook *);
  virtual class Notebook *book() const; // pointer to parent book, if any.
protected:
  virtual void loadMore(QVariantMap const &src);
private: // hide these from general use
  void addChild(Data *);
  bool deleteChild(Data *);
  Data *takeChild(Data *);
  TOCEntry const *lastEntry() const;
private:
  typedef QMap<int, TOCEntry *> EntryMap;
  EntryMap entries_;
  Notebook *nb;
};

#endif
