// CachedEntry.H

#ifndef CACHEDENTRY_H

#define CACHEDENTRY_H

#include "EntryData.H"
#include "EntryFile.H"
#include "CachedPointer.H"

class Entry: public QObject {
public:
  explicit Entry(EntryData *data): data_(data), file_(0) { }
  explicit Entry(EntryFile *file): data_(file->data()), file_(file) { }
  ~Entry() {
    if (file_) {
      file_->saveNow();
      delete file_;
    } else {
      delete data_;
    }
  }   
  EntryData *data() const { return data_; }
  EntryFile *file() const { return file_; }
private:
  EntryData *data_;
  EntryFile *file_;
};

class CachedEntry: public CachedPointer<Entry> {
public:
  explicit CachedEntry() { }
  explicit CachedEntry(EntryData *data): CachedPointer<Entry>(new Entry(data)) { }
  explicit CachedEntry(EntryFile *file): CachedPointer<Entry>(new Entry(file)) { }
  CachedEntry(CachedEntry const &p): CachedPointer<Entry>(p) { }
  CachedEntry const &operator=(CachedEntry const &p) { CachedPointer<Entry>::operator=(p); return *this; }
  virtual ~CachedEntry() {
    qDebug() << "~CachedEntry - saving file";
    if (obj() && obj()->file())
      obj()->file()->saveNow();
  }
public:
  operator EntryData *() const { return obj()->data(); }
  EntryData *operator->() const { return obj()->data(); }
  EntryData *data() const { return obj()->data(); }
  EntryFile *file() const { return obj()->file(); }
};

#endif
