// Index.cpp

#include "Index.H"
#include "WordIndex.H"
#include "Assert.H"
#include <QFile>
#include "EntryData.H"
#include "WordSet.H"

Index::Index(QString rootDir, class TOC *toc, QObject *parent): QObject(parent),
								rootdir(rootDir) {
  widx = new WordIndex(this);
  QString fn = rootdir + "/index.json";
  if (QFile(fn).exists()) {
    widx->load(fn);
  } else {
    widx->build(toc, rootdir + "/pages");
    widx->save(fn);
  }
}

Index::~Index() {
}

void Index::watchEntry(EntryData *e) {
  ASSERT(e);
  int pgno = e->startPage();
  if (pgs.contains(pgno))
    pgs[pgno]->detach();
  else
    pgs[pgno] = new WordSet(this);
  pgs[pgno]->attach(e);  
}

void Index::unwatchEntry(EntryData *e) {
  ASSERT(e);
  int pgno = e->startPage();
  if (pgs.contains(pgno)) {
    flush(pgno);
    delete pgs[pgno];
    pgs.remove(pgno);
  }  
}

void Index::deleteEntry(EntryData *e) {
 ASSERT(e);
  int pgno = e->startPage();
  if (pgs.contains(pgno)) {
    delete pgs[pgno];
    pgs.remove(pgno);
  }
  words()->dropEntry(pgno);
}

void Index::flush() {
  bool needToSave = false;
  foreach (int pgno, pgs.keys())
    needToSave |= flush(pgno);
  if (needToSave)
    words()->save(rootdir + "/index.json");
}

bool Index::flush(int pgno) {
  ASSERT(pgs.contains(pgno));
  WordSet *s = pgs[pgno];
  if (s->outOfDate()) {
    words()->rebuildEntry(pgno, s);
    return true;
  } else {
    return false;
  }
}
  

WordIndex *Index::words() const {
  return widx;
}
