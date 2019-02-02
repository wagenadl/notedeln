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
#include "ElnAssert.h"
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QSignalMapper>
#include "EntryFile.h"
#include "LateNoteManager.h"

#define INDEX_SAVEIVAL_S 5

Index::Index(QString rootDir, class TOC *toc, QObject *parent):
  QObject(parent), rootdir(rootDir) {
  widx = new WordIndex(this);
  mp = new QSignalMapper(this);
  saveTimer = new QTimer(this);
  connect(saveTimer, SIGNAL(timeout()), SLOT(flush()));
  connect(mp, SIGNAL(mapped(QObject*)), SLOT(updateEntry(QObject*)));
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
  connect(f, SIGNAL(saved()), mp, SLOT(map()), Qt::UniqueConnection);
  connect(e->lateNoteManager(), SIGNAL(mod()),
	  mp, SLOT(map()), Qt::UniqueConnection);
  oldsets[pgno] = e->wordSet();
  mp->setMapping(f, e);
  mp->setMapping(e->lateNoteManager(), e);
}

void Index::unwatchEntry(Entry *e) {
  ASSERT(e);
  EntryData *d = e->data();
  ASSERT(d);
  EntryFile *f = e->file();
  ASSERT(f);
  int pgno = d->startPage();
  disconnect(f, SIGNAL(saved()), mp, SLOT(map()));
  disconnect(e->lateNoteManager(), SIGNAL(mod()), mp, SLOT(map()));
  mp->removeMappings(f);
  mp->removeMappings(e->lateNoteManager());
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
