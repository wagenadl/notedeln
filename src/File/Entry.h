// Entry.H

#ifndef ENTRY_H

#define ENTRY_H

#include <QObject>
#include "EntryData.h"
#include "EntryFile.h"

class Entry: public QObject {
public:
  explicit Entry(EntryData *data);
  explicit Entry(EntryFile *file);
  ~Entry();
  EntryData *data() const;
  EntryFile *file() const;
private:
  EntryData *data_;
  EntryFile *file_;
};

#endif
