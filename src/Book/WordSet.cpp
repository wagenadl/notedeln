// WordSet.cpp - This file is part of eln

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

#include "WordSet.H"
#include "TextData.H"

WordSet::WordSet(QObject *parent): QObject(parent) {
  outofdate = false;
}

WordSet::~WordSet() {
}

void WordSet::clear() {
  detach();
  words.clear();
  outofdate = false;
}

void WordSet::detach() {
  if (src)
    disconnect(src, SIGNAL(mod()), this, SLOT(datamod()));
  src = 0;
}

void WordSet::attach(Data *data) {
  src = data;
  add(src);
  origs = toSet();
  outofdate = false;
  connect(src, SIGNAL(mod()), SLOT(datamod()));
}

void WordSet::refresh() {
  words.clear();
  add(src);
  outofdate = false;
}

void WordSet::add(Data const *src) {
  if (!src)
    return;

  TextData const *srct = dynamic_cast<TextData const *>(src);
  if (srct) {
    foreach (QString w, srct->text().split(QRegExp("\\W+"))) {
      w = w.toLower();
      if (w.contains(QRegExp("[a-z]"))) {
	words[w]++;
      }
    }
  }
  
  foreach (Data const *ch, src->allChildren()) 
    add(ch);
}

int WordSet::count(QString w) {
  if (outofdate)
    refresh();
  if (words.contains(w))
    return words[w];
  else
    return 0;
}

QSet<QString> const &WordSet::originalSet() const {
  return origs;
}

QSet<QString> WordSet::toSet()  {
  if (outofdate)
    refresh();
  QSet<QString> s;
  foreach (QString w, words.keys())
    s << w;
  return s;
}

QStringList WordSet::toList() {
  if (outofdate)
    refresh();
  QStringList s;
  foreach (QString w, words.keys())
    s << w;
  return s;
}
  
void WordSet::datamod() {
  outofdate = true;
}

bool WordSet::outOfDate() const {
  return outofdate;
}
