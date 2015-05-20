// Entry.H

#ifndef ENTRY_H

#define ENTRY_H

#include <QObject>
#include "EntryData.h"
#include "EntryFile.h"

class Entry: public QObject {
public:
  /* It is allowable for data and/or file to be null. */
  explicit Entry(EntryData *data);
  explicit Entry(EntryFile *file);
  ~Entry();
  EntryData *data() const; // will throw exception rather than return null
  EntryFile *file() const; // will throw exception rather than return null
  bool isValid() const { return data_!=0; }
  bool hasFile() const { return file_!=0; }
private:
  EntryData *data_;
  EntryFile *file_;
};

#endif
