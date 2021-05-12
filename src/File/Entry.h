// File/Entry.h - This file is part of NotedELN

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

// Entry.H

#ifndef ENTRY_H

#define ENTRY_H

#include <QObject>
#include "EntryData.h"
#include "EntryFile.h"

class Entry: public QObject {
  Q_OBJECT;
public:
  /* It is allowable for data and/or file to be null. */
  explicit Entry(EntryData *data);
  explicit Entry(EntryFile *file);
  ~Entry();
  EntryData *data() const; // will throw exception rather than return null
  EntryFile *file() const; // will throw exception rather than return null
  class LateNoteManager *lateNoteManager() const; // ditto
  bool isValid() const { return data_!=0; }
  bool hasFile() const { return file_!=0; }
  bool saveNow() const;
  bool needToSave() const;
  void setBook(class Notebook *);
  QSet<QString> wordSet() const; // does *not* ensure that late notes are loaded
private:
  EntryData *data_;
  EntryFile *file_;
  LateNoteManager *lnm_;
};

#endif
