// CachedEntry.H

#ifndef CACHEDENTRY_H

#define CACHEDENTRY_H

#include "CachedPointer.H"
#include "Entry.H"

class CachedEntry: public CachedPointer<Entry> {
public:
  explicit CachedEntry();
  explicit CachedEntry(EntryData *data);
  explicit CachedEntry(EntryFile *file);
  CachedEntry(CachedEntry const &p);
  CachedEntry const &operator=(CachedEntry const &p);
  virtual ~CachedEntry();
public:
  operator EntryData *() const;
  EntryData *operator->() const;
  EntryData *data() const;
  EntryFile *file() const;
};

#endif
