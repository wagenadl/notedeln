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

#include "WordIndex.H"
#include "Notebook.H"
#include "TOC.H"
#include "EntryFile.H"
#include "EntryData.H"
#include "JSONFile.H"
#include "Assert.H"
#include "WordSet.H"

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
  index.clear();
  for (QVariantMap::iterator i = idx.begin(); i!=idx.end(); i++) {
    QString w = i.key();
    QVariantList lst = i.value().toList();
    for (QVariantList::iterator j = lst.begin(); j!=lst.end(); j++) {
      index[w].insert((*j).toInt());
    }
  }
  return true;
}

bool WordIndex::save(QString filename) {
  QVariantMap idx;
  for (MapType::iterator i = index.begin(); i!=index.end(); i++) {
    QString w = i.key();
    QVariantList lst;
    foreach (int n, i.value())
      lst.append(QVariant(n));
    idx[w] = QVariant(lst);
  }
  return JSONFile::save(idx, filename);
}

void WordIndex::build(class TOC *toc, QString pagesDir) {
  index.clear();
  foreach (int pg, toc->entries().keys()) {
    EntryFile *f = ::loadPage(pagesDir, pg, 0);
    WordSet ws;
    ws.add(f->data());
    foreach (QString w, ws.toSet())
      index[w].insert(pg);
    delete f;
  }  
}

void WordIndex::rebuildEntry(int startPage, WordSet *ws) {
  StringSet s0 = ws->originalSet();
  StringSet s1 = ws->toSet();
  if (s0==s1)
    return;
  StringSet dropped = s0 - s1;
  StringSet added = s1 - s0;
  foreach (QString w, dropped)
    index[w].remove(startPage);
  foreach (QString w, added)
    index[w].insert(startPage);
}

void WordIndex::dropEntry(int startPage) {
  for (MapType::iterator i = index.begin(); i!=index.end(); ++i)
    i.value().remove(startPage);
}

QSet<int> WordIndex::findWord(QString word) {
  if (index.contains(word))
    return index[word];
  else
    return QSet<int>();
}

QSet<int> WordIndex::findPartialWord(QString wordbit) {
  QSet<int> s;
  for (MapType::iterator i = index.begin(); i!=index.end(); ++i)
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