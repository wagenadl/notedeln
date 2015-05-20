// Entry.C

#include "Entry.h"
#include "EntryData.h"
#include "EntryFile.h"
#include <QDebug>
#include "Assert.h"

Entry::Entry(EntryData *data): data_(data), file_(0) {
}

Entry::Entry(EntryFile *file): data_(file ? file->data(): 0), file_(file) {
}

Entry::~Entry() {
  if (file_) {
    file_->saveNow();
    delete file_;
  } else {
    delete data_;
  }
}   

EntryData *Entry::data() const {
  ASSERT(isValid());
  return data_;
}

EntryFile *Entry::file() const {
  ASSERT(hasFile());
  return file_;
}
