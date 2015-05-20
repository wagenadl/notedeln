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
  operator EntryData *() const; // will throw exception rather than return null
  EntryData *operator->() const; // will throw exception rather than return null
  EntryData *data() const; // will throw exception rather than return null
  EntryFile *file() const; // will throw exception rather than return null
};

#endif
