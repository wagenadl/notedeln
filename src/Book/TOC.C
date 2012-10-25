// TOC.C

#include "TOC.H"
#include <QDebug>
#include "TOCEntry.H"

TOC::TOC(Data *parent): Data(parent) {
  setType("toc");
}

void TOC::loadMore(QVariantMap const &src) {
  entries_.clear();
  QVariantMap v = src["pages"].toMap();
  foreach (QString k, v.keys()) {
    int pgno = k.toInt();
    TOCEntry *e = new TOCEntry(this);
    e->load(v[k].toMap());
    entries_[pgno] = e;
  }
}

TOC::~TOC() {
}

QMap<int, TOCEntry *> const &TOC::entries() const {
  return entries_;
}

TOCEntry *TOC::entry(int startPage) const {
  if (entries_.contains(startPage))
    return entries_[startPage];
  else
    return 0;
}

TOCEntry *TOC::newEntry(int startPage) {
  TOCEntry *e = new TOCEntry(this);
  entries_[startPage] = e;
  markModified();
  return e;
}

void TOC::addEntry(int startPage, TOCEntry *e) {
  entries_[startPage] = e;
  markModified();
}

void TOC::saveMore(QVariantMap &dst) const {
  QVariantMap v;
  foreach (int pgno, entries_.keys()) 
    v[QString("%1").arg(pgno)] = QVariant(entries_[pgno]->save());
  dst["pages"] = QVariant(v);
}

bool TOC::contains(int p) const {
  return entries_.contains(p);
}

bool TOC::deleteEntry(int startPage) {
  if (entries_.remove(startPage)) {
    markModified();
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
