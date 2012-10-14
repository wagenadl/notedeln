// TOC.C

#include "TOC.H"
#include <QDebug>
#include "JSONFile.H"

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

TOC *TOC::create(QString fn, QObject *parent) {
  TOC *toc = new TOC(parent);
  toc->fn = fn;
  toc->save();
  if (toc->ok())
    return toc;
  
  delete toc;
  return 0;
}

TOC::TOC(QObject *parent): QObject(parent) {
  // initialize w/o any entries
  ok_ = false;
}

TOC::TOC(QString fn, QObject *parent): QObject(parent), fn(fn) {
  ok_ = false;
  QVariantMap v = JSONFile::load(fn, &ok_);
  if (!ok_) {
    qDebug() << "TOC: Cannot read toc file";
    return;
  }

  foreach (QString k, v.keys()) {
    if (k.startsWith("p")) {
      int pgno = k.mid(1).toInt();
      TOCEntry e;
      e.load(v[k].toMap());
      entries_[pgno] = e;
    } else {
      qDebug() << "Unexpected entry in TOC file";
    }
  }
}

TOC::~TOC() {
}

QMap<int, TOCEntry> const &TOC::entries() const {
  return entries_;
}

void TOC::addEntry(int startPage, TOCEntry const &e) {
  entries_[startPage] = e;
  save();
}

void TOC::setTitle(int startPage, QString title) {
  if (contains(startPage)) {
    entries_[startPage].title = title;
    save();
  } else {
    qDebug() << "TOC::setTitle: unknown page " << startPage;
  }    
}

void TOC::setSheetCount(int startPage, int cnt) {
  if (contains(startPage)) {
    entries_[startPage].sheetCount = cnt;
    save();
  } else {
    qDebug() << "TOC::setSheetCount: unknown page " << startPage;
  }
}

void TOC::save() const {
  QVariantMap v;
  foreach (int pgno, entries_.keys()) 
    v[QString("p%1").arg(pgno)] = QVariant(entries_[pgno].save());
  ok_ = JSONFile::save(v, fn);
}

bool TOC::contains(int p) const {
  return entries_.contains(p);
}

bool TOC::ok() const {
  return ok_;
}
