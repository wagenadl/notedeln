// File/CachedEntry.cpp - This file is part of eln

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

// CachedEntry.cpp

#include "CachedEntry.h"
#include "EntryData.h"
#include "EntryFile.h"
#include <QDebug>


CachedEntry::CachedEntry() {
}

CachedEntry::CachedEntry(EntryData *data):
  CachedPointer<Entry>(new Entry(data)) {
}

CachedEntry::CachedEntry(EntryFile *file):
  CachedPointer<Entry>(new Entry(file)) {
}

CachedEntry::CachedEntry(CachedEntry const &p):
  CachedPointer<Entry>(p) {
}

CachedEntry const &CachedEntry::operator=(CachedEntry const &p) {
  CachedPointer<Entry>::operator=(p);
  return *this;
}

bool CachedEntry::isValid() const {
  return obj() && obj()->isValid();
}

bool CachedEntry::hasFile() const {
  return obj() && obj()->hasFile();
}

CachedEntry::~CachedEntry() {
  if (obj() && obj()->hasFile())
    obj()->file()->saveNow();
}

CachedEntry::operator EntryData *() const {
  return obj()->data();
}

EntryData *CachedEntry::operator->() const {
  return obj()->data();
}

EntryData *CachedEntry::data() const {
  return obj()->data();
}

EntryFile *CachedEntry::file() const {
  return obj()->file();
}

LateNoteManager *CachedEntry::lateNoteManager() const {
  return obj()->lateNoteManager();
}

void CachedEntry::setBook(class Notebook *nb) {
  obj()->setBook(nb);
}
