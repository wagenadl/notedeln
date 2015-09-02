// Book/Index.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// Index.cpp

#include "Index.h"
#include "WordIndex.h"
#include "Assert.h"
#include <QFile>
#include "EntryData.h"
#include "WordSet.h"

Index::Index(QString rootDir, class TOC *toc, QObject *parent):
  QObject(parent), rootdir(rootDir) {
  widx = new WordIndex(this);
  QString fn = rootdir + "/index.json";
  if (QFile(fn).exists()) {
    widx->load(fn);
  } else {
    if (widx->build(toc, rootdir + "/pages"))
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
