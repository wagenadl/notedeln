// Entry.C

#include "Entry.h"
#include "EntryData.h"
#include "EntryFile.h"
#include <QDebug>

Entry::Entry(EntryData *data): data_(data), file_(0) {
}

Entry::Entry(EntryFile *file): data_(file->data()), file_(file) {
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
  return data_;
}

EntryFile *Entry::file() const {
  return file_;
}
