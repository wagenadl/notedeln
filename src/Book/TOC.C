// TOC.C

#include "TOC.H"
#include <QDebug>
#include "JSONFile.H"

static Data::Creator<TOC> c("toc");

TOCEntry::TOCEntry(QString ttl, QDate crea, int npg):
  title(ttl), created(crea), sheetCount(npg) { }

QVariantMap TOCEntry::save() const {
  QVariantMap vm;
  vm["title"] = title;
  vm["date"] = created;
  vm["sheets"] = sheetCount;
  return vm;
}

void TOCEntry::load(QVariantMap const &vm) {
  title = vm["title"].toString();
  created = vm["date"].toDate();
  sheetCount = vm["sheets"].toInt();
}

TOC::TOC(Data *parent): Data(parent) {
  setType("toc");
}

void TOC::loadMore(QVariantMap const &src) {
  entries_.clear();
  QVariantMap v = src["pages"].toMap();
  foreach (QString k, v.keys()) {
    int pgno = k.toInt();
    TOCEntry e;
    e.load(v[k].toMap());
    entries_[pgno] = e;
  }
}

TOC::~TOC() {
}

QMap<int, TOCEntry> const &TOC::entries() const {
  return entries_;
}

void TOC::addEntry(int startPage, TOCEntry const &e) {
  entries_[startPage] = e;
  markModified();
}

void TOC::setTitle(int startPage, QString title) {
  qDebug() << "TOC:setTitle" << startPage << title;
  if (contains(startPage)) {
    entries_[startPage].title = title;
    markModified();
  } else {
    qDebug() << "TOC::setTitle: unknown page " << startPage;
  }    
}

void TOC::setSheetCount(int startPage, int cnt) {
  if (contains(startPage)) {
    entries_[startPage].sheetCount = cnt;
    markModified();
  } else {
    qDebug() << "TOC::setSheetCount: unknown page " << startPage;
  }
}

void TOC::saveMore(QVariantMap &dst) const {
  QVariantMap v;
  foreach (int pgno, entries_.keys()) 
    v[QString("%1").arg(pgno)] = QVariant(entries_[pgno].save());
  dst["pages"] = QVariant(v);
}

bool TOC::contains(int p) const {
  return entries_.contains(p);
}
