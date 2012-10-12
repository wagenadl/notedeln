// TOC.C

#include "TOC.H"

TOC::Entry::Entry(int startPage, QString fn, QString title):
  startPage(startPage), fn(fn), title(title) {
}

bool TOC::Entry::operator<(TOC::Entry const &o) const {
  if (startPage < o.startPage)
    return true;
  else if (startPage > o.startPage)
    return false;
  if (fn < o.fn)
    return true;
  else if (fn > o.fn)
    return false;
  return title < o.title;
}

QVariantMap TOC::Entry::save() const {
  QVariantMap vm;
  vm["startPage"] = startPage;
  vm["fn"] = fn;
  vm["title"] = title;
  return vm;
}

void TOC::Entry::load(QVariantMap const &vm) {
  startPage = vm["startPage"].toInt();
  fn = vm["fn"].toString();
  title = vm["title"].toString();
}

TOC *TOC::create(QString fn, QObject *parent) {
  TOC *toc = new TOC(parent);
  toc->fn = fn;
  if (!toc->save())
    delete toc;
  return toc;
}

TOC::TOC(QObject *parent): QObject(parent) {
  // initialize w/o any entries
}

TOC::TOC(QString fn, QObject *parent): QObject(parent), fn(fn) {
  QFile f(fn);
  if (!f.open(QFile::ReadOnly)) {
    qDebug() << "TOC: Cannot read toc file";
    return;
  }
  
  QJson::Parser parser;
  bool ok;
  QVariant doc = parser.parse(&f, &ok);
  f.close();
  if (!ok) {
    qDebug() << "TOC: Parse failed: "
	     << parser.errorString()
	     << " at line " << parser.errorLine();
    return;
  }

  foreach (QVariant v, doc.toMap()["entries"].toList()) {
    Entry e;
    e.load(v.toMap());
    entries_.append(e);
  }
  qSort(entries_.begin(), entries_.end());
}

TOC::~TOC() {
}

QList<Entry> const &entries() const {
  return entries_;
}

void addEntry(int startPage, QString fn, QString title) {
  Entry e;
  e.startPage = startPage;
  e.fn = fn;
  e.title = title;
  for (QList<Entry>::iterator i=entries_.begin(); i!=entries_.end(); ++i) {
    if (e < *i) {
      entries_.insert(i, e);
      save();
      return;
    }
  }
  entries_.append(e);
  save();
}

void setStartPage(QString fn, int startPage) {
  foreach (Entry &e, entries_)
    if (e.fn == fn)
      e.startPage = startPage;
  qSort(entries.begin(), entries.end());
  save();
}

void setTitle(QString fn, QString title) {
  foreach (Entry &e, entries_)
    if (e.fn == fn)
      e.title = title;
  qSort(entries.begin(), entries.end());
  save();
}

bool TOC::save() {
  QVariantMap vm;
  QVariantList vl;
  foreach (Entry const &e, entries_)
    vl.append(e.save());
  vm["entries"] = vl;
  QJson::Serialiser s;
  QByteArray ba = s.serialize(QVariant(vm));
  if (ba.isEmpty()) {
    qDebug() << "TOC: serialization failed";
    return false;
  }

  QFile f(fn_);
  
  if (f.exists()) {
    QFile f0(fn_ + "~");
    if (f0.exists())
      qDebug() << "(TOC: Removing ancient file)";
    f0.remove();
    qDebug() << "(TOC: Renaming old file)";
    f.rename(fn_ + "~");
    f.setFileName(fn_);
  }
  
  if (!f.open(QFile::WriteOnly)) {
    qDebug() << "TOC: Cannot open file for writing";
    return false;
  }

  if (f.write(ba) != ba.size()) {
    qDebug() << "TOC: Failed to write all contents";
    return false;
  }

  f.close();

  return true;
}

