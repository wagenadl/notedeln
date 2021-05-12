// File/CachedEntry.h - This file is part of NotedELN

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

// CachedEntry.H

#ifndef CACHEDENTRY_H

#define CACHEDENTRY_H

#include "CachedPointer.h"
#include "Entry.h"

class CachedEntry: public CachedPointer<Entry> {
public:
  explicit CachedEntry();
  explicit CachedEntry(EntryData *data);
  explicit CachedEntry(EntryFile *file);
  CachedEntry(CachedEntry const &p);
  CachedEntry const &operator=(CachedEntry const &p);
  virtual ~CachedEntry();
public:
  bool isValid() const;
  bool hasFile() const;
  bool saveNow() const; // true if OK
  bool needToSave() const; // true if need
  operator EntryData *() const; // will throw exception rather than return null
  EntryData *operator->() const; // will throw exception rather than return null
  EntryData *data() const; // will throw exception rather than return null
  EntryFile *file() const; // will throw exception rather than return null
  class LateNoteManager *lateNoteManager() const; // ditto
  void setBook(class Notebook *);
};

#endif
