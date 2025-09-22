// Book/Index.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Index.cpp

#include "Index.h"
#include "WordIndex.h"
#include "ElnAssert.h"
#include <QDebug>
#include <QFile>
#include <QTimer>
#include "EntryFile.h"
#include "LateNoteManager.h"

#define INDEX_SAVEIVAL_S 5

Index::Index(QString rootDir, class TOC *toc, QObject *parent):
  QObject(parent), rootdir(rootDir) {
  widx = new WordIndex(this);
  saveTimer = new QTimer(this);
  connect(saveTimer, &QTimer::timeout, this, &Index::flush);
  QString fn = rootdir + "/index.json";
  if (QFile(fn).exists()) {
    widx->load(fn);
    if (widx->update(toc, rootdir + "/pages"))
      widx->save(fn);
  } else {
    if (widx->build(toc, rootdir + "/pages"))
      widx->save(fn);
  }
  needToSave = false;
}

Index::~Index() {
  flush();
}

void Index::watchEntry(Entry *e) {
  ASSERT(e);
  EntryData *d = e->data();
  ASSERT(d);
  EntryFile *f = e->file();
  ASSERT(f);
  int pgno = d->startPage();
  if (!cons.contains(f))
    cons[f] = connect(f, &EntryFile::saved,
                      this, [this, e]() { updateEntry(e); });
  LateNoteManager *lnm = e->lateNoteManager();
  if (lnm && !cons.contains(lnm))
    cons[lnm] = connect(lnm, &LateNoteManager::mod,
                        this, [this, e]() { updateEntry(e); });
  oldsets[pgno] = e->wordSet();
}

void Index::unwatchEntry(Entry *e) {
  ASSERT(e);
  EntryData *d = e->data();
  ASSERT(d);
  EntryFile *f = e->file();
  ASSERT(f);
  LateNoteManager *lnm = e->lateNoteManager();
  int pgno = d->startPage();
  if (cons.contains(f))
    disconnect(cons[f]);
  cons.remove(f);
  if (lnm && cons.contains(lnm))
    disconnect(cons[lnm]);
  cons.remove(lnm);
  oldsets.remove(pgno);
}

void Index::deleteEntry(Entry *e) {
  ASSERT(e);
  EntryData *d = e->data();
  ASSERT(d);
  int pgno = d->startPage();
  words()->dropEntry(pgno);
  unwatchEntry(e);
}

void Index::flush() {
  saveTimer->stop();
  if (needToSave)
    words()->save(rootdir + "/index.json");
  needToSave = false;
}

WordIndex *Index::words() const {
  return widx;
}

void Index::updateEntry(QObject *obj) {
  Entry *e = dynamic_cast<Entry *>(obj);
  ASSERT(e);
  EntryData *d = e->data();
  ASSERT(d);
  QSet<QString> words = e->wordSet();
  int pgno = d->startPage();

  if (words!=oldsets[pgno]) {
    widx->rebuildEntry(pgno, words, &oldsets[pgno]);
    oldsets[pgno] = words;
    needToSave = true;
    saveTimer->start(INDEX_SAVEIVAL_S * 1000);
  }
}
