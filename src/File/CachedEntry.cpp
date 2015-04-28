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

CachedEntry::~CachedEntry() {
  if (obj() && obj()->file())
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
