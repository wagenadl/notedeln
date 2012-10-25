// TOCEntry.C

#include "TOCEntry.H"

static Data::Creator<TOCEntry> c("entry");

TOCEntry::TOCEntry(Data *parent): Data(parent) {
  setType("entry");
  sheetCount_ = 1;
}

TOCEntry::~TOCEntry() {
}

QString TOCEntry::title() const {
  return title_;
}

int TOCEntry::sheetCount() const {
  return sheetCount_;
}

void TOCEntry::setTitle(QString t) {
  title_ = t;
  markModified();
}

void TOCEntry::setSheetCount(int n) {
  sheetCount_ = n;
  markModified();
}
