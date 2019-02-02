// WordIndex.cpp - This file is part of eln

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

#include "WordIndex.h"
#include "Notebook.h"
#include "TOC.h"
#include "EntryFile.h"
#include "EntryData.h"
#include "JSONFile.h"
#include "ElnAssert.h"
#include "Translate.h"
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include "LateNoteManager.h"

WordIndex::WordIndex(QObject *parent): QObject(parent) {
}

WordIndex::~WordIndex() {
}

/* Index is saved in json as a map from words to an array of integers. */

bool WordIndex::load(QString filename) {
  bool ok;
  QVariantMap idx = JSONFile::load(filename, &ok);
  if (!ok)
    return false;

  if (idx.contains("vsn no")) {
    buildIndex(idx["index"].toMap());
    lastseen.clear();
    QVariantMap ls(idx["ls"].toMap());
    for (QVariantMap::iterator i = ls.begin(); i!=ls.end(); i++) {
      int pg = i.key().toInt();
      QDateTime dt = i.value().toDateTime();
      lastseen[pg] = dt;
    }     
  } else {
    lastseen.clear();
    buildIndex(idx);
  }

  return true;
}

void WordIndex::buildIndex(QVariantMap const &idx) {
  index.clear();

  for (auto i = idx.begin(); i!=idx.end(); i++) {
    QString w = i.key();
    QVariantList lst = i.value().toList();
    for (QVariantList::iterator j = lst.begin(); j!=lst.end(); j++) {
      int pg = (*j).toInt();
      index[w].insert(pg);
    }
  }
}

bool WordIndex::save(QString filename) {
  QVariantMap idx;
  for (auto i = index.begin(); i!=index.end(); i++) {
    QString w = i.key();
    QVariantList lst;
    foreach (int n, i.value())
      lst.append(QVariant(n));
    idx[w] = QVariant(lst);
  }

  QVariantMap ls;
  // qDebug() << "Saving index. sizeof lastseen is" << lastseen.size();
  for (auto i=lastseen.begin(); i!=lastseen.end(); i++) {
    int pgno = i.key();
    QDateTime dt = i.value();
    ls[QString::number(pgno)] = QVariant(dt);
  }

  QVariantMap top;
  top["vsn no"] = 1;
  top["index"] = idx;
  top["ls"] = ls;
  
  return JSONFile::save(top, filename, true);
}

bool WordIndex::build(class TOC *toc, QString pagesDir) {
  QProgressDialog mb("Search index found missing or corrupted."
                     " Rebuilding...", "Cancel",
                     0, toc->newPageNumber());
  mb.setWindowModality(Qt::WindowModal);
  mb.setMinimumDuration(200);
  mb.setValue(0);
  
  index.clear();
  QStringList warns;
  foreach (int pg, toc->entries().keys()) {
    if (mb.wasCanceled())
      return false;
    QString uuid = toc->tocEntry(pg)->uuid();
    EntryFile *f = ::loadEntry(pagesDir, pg, uuid, 0);
    if (f) {
      Entry *entry = new Entry(f);
      entry->lateNoteManager()->ensureLoaded();
      for (QString w: entry->wordSet())
        index[w].insert(pg);
      delete entry;
    } else {
      qDebug() << "WordIndex::build - Cannot load entry" << pg << uuid;
      warns << QString("%1").arg(pg);
    }
    lastseen[pg] = QDateTime::currentDateTime();
    qDebug() << "lastseen " << pg << lastseen[pg];
    mb.setValue(pg);
  } 
  mb.close();
  if (!warns.isEmpty())
    QMessageBox::warning(0, Translate::_("eln"),
                         "The following pages could not be loaded"
                         " while rebuilding the search index: "
                         + warns.join(", ") + ".", QMessageBox::Close);
  return true;
}

void WordIndex::rebuildEntry(int startPage, QSet<QString> newset,
                             QSet<QString> *oldset) {
  lastseen[startPage] = QDateTime::currentDateTime();
  
  if (oldset) {
    QSet<QString> dropped = *oldset - newset;
    QSet<QString> added = newset - *oldset;
    foreach (QString w, dropped) {
      index[w].remove(startPage);
      if (index[w].isEmpty())
        index.remove(w);
    }
    foreach (QString w, added)
      index[w].insert(startPage);
  } else {
    dropEntry(startPage);
    for (QString w: newset)
      index[w].insert(startPage);
  }
}

void WordIndex::dropEntry(int startPage) {
  lastseen.remove(startPage);
  for (QSet<int> &set: index)
    set.remove(startPage);
}

QSet<int> WordIndex::findWord(QString word) {
  if (index.contains(word))
    return index[word];
  else
    return QSet<int>();
}

QSet<int> WordIndex::findPartialWord(QString wordbit) {
  QSet<int> s;
  for (auto i = index.begin(); i!=index.end(); ++i)
    if (i.key().startsWith(wordbit))
      s |= i.value();
  return s;
}

QSet<int> WordIndex::findWords(QStringList words, bool lastPartial) {
  QSet<int> s;
  if (words.isEmpty())
    return s;

  QString w = words.takeLast();
  if (lastPartial)
    s = findPartialWord(w);
  else
    s = findWord(w);

  foreach (w, words) 
    s &= findWord(w);

  return s;
}

bool WordIndex::update(TOC const *toc, QString pagesDir) {
  QList<TOCEntry const *> todo;
  for (TOCEntry const *entry: toc->entries()) {
    int pg = entry->startPage();
    QDateTime mod = entry->modified();
    if (!lastseen.contains(pg) || mod>lastseen[pg].addSecs(10))
      todo << entry;
  }
  
  if (todo.isEmpty())
    return false;

  QProgressDialog mb("Updating search index...", "Cancel",
                     0, todo.size());
  mb.setWindowModality(Qt::WindowModal);
  mb.setMinimumDuration(200);

  int k = 0;
  QStringList warns;
  for (TOCEntry const *entry: todo) {
    if (mb.wasCanceled())
      return k>0;
    int pgno = entry->startPage();
    QString uuid = entry->uuid();
    EntryFile *f = ::loadEntry(pagesDir, pgno, uuid, 0);
    if (f) {
      for (QString w: f->data()->wordSet())
        index[w].insert(pgno);
      delete f;
      lastseen[pgno] = QDateTime::currentDateTime();
    } else {
      qDebug() << "WordIndex::update - Cannot load entry" << pgno << uuid;
      warns << QString("%1").arg(pgno);
    }
    mb.setValue(++k);
  }
  mb.close();
  if (!warns.isEmpty())
    QMessageBox::warning(0, Translate::_("eln"),
                         "The following pages could not be loaded"
                         " while updating the search index: "
                         + warns.join(", ") + ".", QMessageBox::Close);
  return true;
}
  

    
