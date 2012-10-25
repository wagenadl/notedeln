// TOC.C

#include "TOC.H"
#include <QDebug>
#include "TOCEntry.H"
#include "PageData.H"
#include "TitleData.H"

TOC::TOC(Data *parent): Data(parent) {
  setType("toc");
}

void TOC::loadMore(QVariantMap const &) {
  entries_.clear();
  foreach (TOCEntry *e, children<TOCEntry>()) 
    entries_[e->startPage()] = e;
}

TOC::~TOC() {
}

QMap<int, TOCEntry *> const &TOC::entries() const {
  return entries_;
}

TOCEntry *TOC::entry(int startPage) const {
  Q_ASSERT(entries_.contains(startPage));
  return entries_[startPage];
}

TOCEntry *TOC::find(int page) const {
  // This is not an efficient implementation, but it's fine for reasonably
  // sized notebooks
  foreach (TOCEntry *e, entries_) 
    if (page >= e->startPage() && page <= e->startPage() + e->sheetCount())
      return e;
  return 0;
}

TOCEntry *TOC::addEntry(PageData *data) {
  TOCEntry *e = new TOCEntry(this);
  e->setStartPage(data->startPage());
  e->setTitle(data->titleText());
  e->setSheetCount(data->sheetCount());
  entries_[e->startPage()] = e;
  Data::addChild(e);
  return e;
}

bool TOC::contains(int p) const {
  return entries_.contains(p);
}

bool TOC::deleteEntry(TOCEntry *e) {
  if (!e)
    return 0;
  int p = e->startPage();
  if (entries_.remove(p)) {
    Data::deleteChild(e);
    return true;
  } else {
    return false;
  }
}

int TOC::newPageNumber() const {
  if (entries_.isEmpty())
    return 1;
  QMap<int, TOCEntry *>::const_iterator i = entries_.constEnd();
  --i;
  TOCEntry *e = i.value();
  Q_ASSERT(e);
  return i.key() + e->sheetCount();
}

