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
#include "Data.h"
#include "DataFile.h"
#include "TOCEntry.h"
#include <QDir>

class TOC: public Data {
  Q_OBJECT;
public:
  TOC(Data *parent=0);
  virtual ~TOC();
  QMap<int, TOCEntry *> const &entries() const;
  TOCEntry *tocEntry(int startPage) const; // assertion if not found
  TOCEntry *find(int page) const;
  /* Returns entry containing the page page or 0 if none does. */
  TOCEntry *findForward(int page) const;
  /* Returns entry containing the given page or the first entry that
     starts after the given page or 0 if none.
   */
  TOCEntry *findBackward(int page) const;
  /* Returns entry containing the given page or the last entry that
     ends before the given page or 0 if none.
   */
  TOCEntry *entryAfter(TOCEntry *te) const; // returns ptr to entry after
           // ... the given entry, or 0 if none exists or if te is invalid.
  bool contains(int startPage) const;
  TOCEntry *addEntry(class EntryData *data);
  bool deleteEntry(TOCEntry *);
  int newPageNumber() const;
  bool isLast(TOCEntry const *) const;
  void setBook(class Notebook *);
  virtual class Notebook *book() const; // pointer to parent book, if any.
  bool verify(QDir pages) const;
public:
  static void readPageDir(QDir pages,
                          QMap<int, QString> &pg2file_out,
                          QMap<int, QString> &pg2uuid_out);
  static TOC *rebuild(QDir pages);
  /* Builds a TOC structure from a pages folder.
     This /only/ works if all the .json files in that folder are readable
     and parsable as entries. Furthermore, there must not be any duplicate
     entries. And there may not be any ".THIS", ".OTHER", ".BASE", or ".moved"
     files in the folder, which would indicate an unsuccessful bzr update.
     Reports graphically to user.
   */
  
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
